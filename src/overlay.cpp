#include "overlay.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
static Overlay* g_self = nullptr;

LRESULT CALLBACK Overlay::WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (ImGui_ImplWin32_WndProcHandler(h, m, w, l)) return 0;
    switch (m) {
        case WM_DESTROY:
            if (g_self) g_self->close_ = true;
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

bool Overlay::create(const wchar_t* targetWindowTitle) {
    g_self = this;

    target_ = FindWindowW(nullptr, targetWindowTitle);

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"madcheats_overlay";
    RegisterClassExW(&wc);

    RECT r{};
    if (target_ && IsWindow(target_)) GetWindowRect(target_, &r);
    else GetWindowRect(GetDesktopWindow(), &r);
    w_ = (r.right - r.left > 0) ? r.right - r.left : 1920;
    h_ = (r.bottom - r.top > 0) ? r.bottom - r.top : 1080;

    hwnd_ = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"madcheats.cc external",
        WS_POPUP,
        r.left, r.top, w_, h_,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd_) return false;

    MARGINS m{ -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd_, &m);
    SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);

    if (!createDevice()) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(dev_, ctx_);

    return true;
}

bool Overlay::createDevice() {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount          = 2;
    sd.BufferDesc.Width     = 0;
    sd.BufferDesc.Height    = 0;
    sd.BufferDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 144;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow         = hwnd_;
    sd.SampleDesc.Count     = 1;
    sd.SampleDesc.Quality   = 0;
    sd.Windowed             = TRUE;
    sd.SwapEffect           = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1
    };
    D3D_FEATURE_LEVEL got;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        levels, (UINT)std::size(levels),
        D3D11_SDK_VERSION, &sd, &swap_, &dev_, &got, &ctx_);
    if (FAILED(hr)) {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
            levels, (UINT)std::size(levels),
            D3D11_SDK_VERSION, &sd, &swap_, &dev_, &got, &ctx_);
        if (FAILED(hr)) return false;
    }
    createRTV();
    return true;
}

void Overlay::createRTV() {
    ID3D11Texture2D* bb = nullptr;
    swap_->GetBuffer(0, IID_PPV_ARGS(&bb));
    if (bb) {
        dev_->CreateRenderTargetView(bb, nullptr, &rtv_);
        bb->Release();
    }
}
void Overlay::releaseRTV() {
    if (rtv_) { rtv_->Release(); rtv_ = nullptr; }
}

void Overlay::syncToTarget() {
    if (!target_ || !IsWindow(target_)) {
        target_ = FindWindowW(nullptr, L"Counter-Strike 2");
        return;
    }
    RECT r;
    if (!GetWindowRect(target_, &r)) return;
    int nw = r.right - r.left;
    int nh = r.bottom - r.top;
    if (nw <= 0 || nh <= 0) return;
    if (nw != w_ || nh != h_) {
        w_ = nw; h_ = nh;
        releaseRTV();
        swap_->ResizeBuffers(0, w_, h_, DXGI_FORMAT_UNKNOWN,
                             DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        createRTV();
    }
    SetWindowPos(hwnd_, HWND_TOPMOST, r.left, r.top, w_, h_,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void Overlay::setClickThrough(bool on) {
    if (on == clickThru_) return;
    clickThru_ = on;
    LONG ex = GetWindowLong(hwnd_, GWL_EXSTYLE);
    if (on)  ex |=  WS_EX_TRANSPARENT;
    else     ex &= ~WS_EX_TRANSPARENT;
    SetWindowLong(hwnd_, GWL_EXSTYLE, ex);
}

bool Overlay::beginFrame() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) close_ = true;
    }
    if (close_) return false;
    syncToTarget();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    return true;
}

void Overlay::endFrame() {
    ImGui::Render();
    const float clear[4] = { 0.f, 0.f, 0.f, 0.f };
    ctx_->OMSetRenderTargets(1, &rtv_, nullptr);
    ctx_->ClearRenderTargetView(rtv_, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    swap_->Present(1, 0);
}

void Overlay::destroy() {
    if (dev_) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    releaseRTV();
    if (swap_) { swap_->Release(); swap_ = nullptr; }
    if (ctx_)  { ctx_->Release();  ctx_  = nullptr; }
    if (dev_)  { dev_->Release();  dev_  = nullptr; }
    if (hwnd_) { DestroyWindow(hwnd_); hwnd_ = nullptr; }
    UnregisterClassW(L"madcheats_overlay", GetModuleHandleW(nullptr));
    g_self = nullptr;
}
