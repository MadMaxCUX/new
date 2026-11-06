#include "aim.hpp"
#include "menu.hpp"
#include <windows.h>
#include <cmath>
#include <algorithm>
#include <limits>

static void mouseNudge(int dx, int dy) {
    if (dx == 0 && dy == 0) return;
    INPUT in{};
    in.type       = INPUT_MOUSE;
    in.mi.dx      = dx;
    in.mi.dy      = dy;
    in.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &in, sizeof(in));
}

void RunAim(const World& world, const ViewMatrix& vm, int myTeam,
            const Vec3& localEye, int sw, int sh) {
    (void)localEye;
    if (!g_cfg.aimEnabled) return;
    if ((GetAsyncKeyState(g_cfg.aimKey) & 0x8000) == 0) return;

    const float cx = sw * 0.5f;
    const float cy = sh * 0.5f;
    const float fovPx = (g_cfg.aimFov / 90.f) * cx;

    float bestDist = std::numeric_limits<float>::max();
    Vec2  bestScreen{};
    bool  found = false;

    int hi = world.highestIndex();
    hi = std::clamp(hi, 32, 128);

    for (int i = 1; i <= hi; ++i) {
        auto op = world.readPlayer(i);
        if (!op) continue;
        const Player& p = *op;
        if (!p.alive) continue;
        if (g_cfg.aimTeamSafe && p.team == myTeam) continue;

        int boneIdx = std::clamp(g_cfg.aimBone, 0, (int)p.bones.size() - 1);
        Vec3 target = p.bones[boneIdx];
        if (!target.valid()) continue;

        Vec2 s{};
        if (!WorldToScreen(vm, target, sw, sh, s)) continue;

        float dx = s.x - cx, dy = s.y - cy;
        float d = std::sqrt(dx*dx + dy*dy);
        if (d > fovPx) continue;

        if (d < bestDist) {
            bestDist = d;
            bestScreen = s;
            found = true;
        }
    }

    if (!found) return;

    float smooth = std::max(1.f, g_cfg.aimSmooth);
    float dx = (bestScreen.x - cx) / smooth;
    float dy = (bestScreen.y - cy) / smooth;

    dx = std::clamp(dx, -200.f, 200.f);
    dy = std::clamp(dy, -200.f, 200.f);

    int idx = (int)std::lround(dx);
    int idy = (int)std::lround(dy);
    mouseNudge(idx, idy);
}
