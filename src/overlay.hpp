#pragma once
#include <windows.h>
#include <d3d11.h>

class Overlay {
public:
    bool create(const wchar_t* targetWindowTitle);
    void destroy();

    bool beginFrame();
    void endFrame();

    HWND hwnd()   const { return hwnd_; }
    int  width()  const { return w_; }
    int  height() const { return h_; }
    bool wantClose() const { return close_; }

    void setClickThrough(bool on);

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
    HWND hwnd_      = nullptr;
    HWND target_    = nullptr;
    int  w_         = 1920;
    int  h_         = 1080;
    bool close_     = false;
    bool clickThru_ = true;

    ID3D11Device*            dev_  = nullptr;
    ID3D11DeviceContext*     ctx_  = nullptr;
    IDXGISwapChain*          swap_ = nullptr;
    ID3D11RenderTargetView*  rtv_  = nullptr;

    bool createDevice();
    void createRTV();
    void releaseRTV();
    void syncToTarget();
};
