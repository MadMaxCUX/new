#pragma once
#include <imgui.h>
#include <windows.h>

struct Config {
    // ui
    bool  menuOpen        = true;

    // esp
    bool  espEnabled      = true;
    bool  espBox          = true;
    bool  espName         = true;
    bool  espHealthBar    = true;
    bool  espHealthText   = true;
    bool  espDistance     = true;
    bool  espSnapline     = false;
    bool  espSkeleton     = false;
    bool  espEnemyOnly    = true;
    bool  espWeapon       = false;
    float maxDistance     = 3500.0f;
    float boxThickness    = 1.5f;
    ImVec4 colEnemy       = {1.00f, 0.22f, 0.28f, 1.0f};
    ImVec4 colTeam        = {0.20f, 0.90f, 1.00f, 1.0f};
    ImVec4 colText        = {0.95f, 0.95f, 0.95f, 1.0f};

    // aim
    bool  aimEnabled      = true;
    float aimFov          = 6.0f;
    float aimSmooth       = 4.0f;
    int   aimBone         = 6;        // HEAD
    int   aimKey          = VK_XBUTTON2;
    bool  aimTeamSafe     = true;
    bool  aimVisibleOnly  = false;
    bool  aimDrawFov      = true;

    // misc
    bool  crosshair       = false;
};

inline Config g_cfg;

void DrawMenu();
void DrawCrosshair(int sw, int sh);
void DrawFovCircle(int sw, int sh);
