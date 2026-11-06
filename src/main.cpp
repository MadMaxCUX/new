#include "overlay.hpp"
#include "process.hpp"
#include "sdk.hpp"
#include "esp.hpp"
#include "aim.hpp"
#include "menu.hpp"
#include <windows.h>
#include <thread>
#include <atomic>
#include <chrono>

static std::atomic<bool> g_run{true};

static void waitForCS2(Process& proc) {
    MessageBoxW(nullptr,
        L"Waiting for Counter-Strike 2.\n"
        L"Start the game, then press OK.",
        L"madcheats.cc",
        MB_ICONINFORMATION | MB_OK | MB_TOPMOST);
    for (int i = 0; i < 60 && !proc.attach(L"cs2.exe"); ++i)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    Process proc;
    if (!proc.attach(L"cs2.exe")) waitForCS2(proc);
    if (!proc.pid()) {
        MessageBoxW(nullptr, L"cs2.exe never showed up.", L"madcheats.cc", MB_ICONERROR);
        return 1;
    }

    std::uintptr_t client = 0;
    for (int i = 0; i < 30 && client == 0; ++i) {
        client = proc.moduleBase(L"client.dll");
        if (!client) std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (!client) {
        MessageBoxW(nullptr,
            L"client.dll not mapped yet. Wait until the main menu is visible, then relaunch.",
            L"madcheats.cc", MB_ICONERROR);
        return 2;
    }

    World world;
    world.bind(&proc, client);

    Overlay ov;
    if (!ov.create(L"Counter-Strike 2")) {
        MessageBoxW(nullptr, L"overlay creation failed.", L"madcheats.cc", MB_ICONERROR);
        return 3;
    }

    std::thread aimer([&]{
        while (g_run.load()) {
            if (!proc.alive()) { g_run.store(false); break; }
            auto vm   = world.viewMatrix();
            int  team = world.localTeam();
            Vec3 eye  = world.localEye();
            RunAim(world, vm, team, eye, ov.width(), ov.height());
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    });

    bool prevInsert = false, prevEnd = false;
    while (!ov.wantClose() && g_run.load()) {
        bool insert = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        bool endK   = (GetAsyncKeyState(VK_END)    & 0x8000) != 0;
        if (insert && !prevInsert) {
            g_cfg.menuOpen = !g_cfg.menuOpen;
            ov.setClickThrough(!g_cfg.menuOpen);
            if (g_cfg.menuOpen) SetForegroundWindow(ov.hwnd());
        }
        if (endK && !prevEnd) break;
        prevInsert = insert; prevEnd = endK;

        if (!proc.alive()) break;
        if (!ov.beginFrame()) break;

        auto vm   = world.viewMatrix();
        int  team = world.localTeam();
        Vec3 eye  = world.localEye();

        DrawFovCircle(ov.width(), ov.height());
        DrawESP(world, vm, team, eye, ov.width(), ov.height());
        DrawCrosshair(ov.width(), ov.height());
        DrawMenu();

        ov.endFrame();
    }

    g_run.store(false);
    if (aimer.joinable()) aimer.join();
    ov.destroy();
    proc.detach();
    return 0;
}
