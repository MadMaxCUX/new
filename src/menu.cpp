#include "menu.hpp"
#include <imgui.h>

static const char* kBones[] = {
    "pelvis", "spine 1", "spine 2", "chest", "upper chest",
    "neck", "head", "l clavicle", "l shoulder", "l elbow",
    "l wrist", "r clavicle", "r shoulder", "r elbow", "r wrist", "root"
};

static const char* kAimKeys[] = { "mouse 4", "mouse 5", "shift", "alt", "ctrl", "c", "e" };
static const int   kAimKeyVK[] = { VK_XBUTTON1, VK_XBUTTON2, VK_SHIFT, VK_MENU, VK_CONTROL, 'C', 'E' };

static int keyToIndex(int vk) {
    for (int i = 0; i < (int)(sizeof(kAimKeyVK)/sizeof(kAimKeyVK[0])); ++i)
        if (kAimKeyVK[i] == vk) return i;
    return 1;
}

static void StyleDark() {
    auto& s = ImGui::GetStyle();
    s.WindowRounding   = 8.0f;
    s.FrameRounding    = 4.0f;
    s.GrabRounding     = 4.0f;
    s.ScrollbarRounding = 4.0f;
    s.TabRounding      = 4.0f;
    s.WindowPadding    = ImVec2(12, 12);
    s.FramePadding     = ImVec2(8, 4);
    s.ItemSpacing      = ImVec2(8, 6);

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]       = ImVec4(0.06f, 0.06f, 0.08f, 0.94f);
    c[ImGuiCol_TitleBg]        = ImVec4(0.10f, 0.03f, 0.14f, 1.00f);
    c[ImGuiCol_TitleBgActive]  = ImVec4(0.14f, 0.05f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBg]        = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.08f, 0.28f, 1.00f);
    c[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.10f, 0.40f, 1.00f);
    c[ImGuiCol_CheckMark]      = ImVec4(0.95f, 0.32f, 0.58f, 1.00f);
    c[ImGuiCol_SliderGrab]     = ImVec4(0.90f, 0.30f, 0.55f, 1.00f);
    c[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.40f, 0.70f, 1.00f);
    c[ImGuiCol_Button]         = ImVec4(0.18f, 0.08f, 0.26f, 1.00f);
    c[ImGuiCol_ButtonHovered]  = ImVec4(0.30f, 0.14f, 0.42f, 1.00f);
    c[ImGuiCol_ButtonActive]   = ImVec4(0.50f, 0.20f, 0.65f, 1.00f);
    c[ImGuiCol_Tab]            = ImVec4(0.12f, 0.06f, 0.18f, 1.00f);
    c[ImGuiCol_TabHovered]     = ImVec4(0.32f, 0.14f, 0.48f, 1.00f);
    c[ImGuiCol_TabActive]      = ImVec4(0.24f, 0.10f, 0.36f, 1.00f);
    c[ImGuiCol_Header]         = ImVec4(0.18f, 0.08f, 0.28f, 1.00f);
    c[ImGuiCol_HeaderHovered]  = ImVec4(0.30f, 0.14f, 0.44f, 1.00f);
    c[ImGuiCol_HeaderActive]   = ImVec4(0.42f, 0.18f, 0.56f, 1.00f);
}

void DrawMenu() {
    static bool styled = false;
    if (!styled) { StyleDark(); styled = true; }
    if (!g_cfg.menuOpen) return;

    ImGui::SetNextWindowSize(ImVec2(540, 420), ImGuiCond_Once);
    ImGui::SetNextWindowPos (ImVec2(80, 80),  ImGuiCond_Once);
    ImGui::Begin("madcheats.cc  external", &g_cfg.menuOpen,
                 ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("##tabs")) {
        if (ImGui::BeginTabItem("esp")) {
            ImGui::Checkbox("enabled",          &g_cfg.espEnabled);
            ImGui::Separator();
            ImGui::Checkbox("box",              &g_cfg.espBox);
            ImGui::SameLine(); ImGui::SetNextItemWidth(120);
            ImGui::SliderFloat("##thick",       &g_cfg.boxThickness, 0.5f, 4.f, "%.1f px");
            ImGui::Checkbox("name",             &g_cfg.espName);
            ImGui::Checkbox("health bar",       &g_cfg.espHealthBar);
            ImGui::Checkbox("health text",      &g_cfg.espHealthText);
            ImGui::Checkbox("distance",         &g_cfg.espDistance);
            ImGui::Checkbox("weapon",           &g_cfg.espWeapon);
            ImGui::Checkbox("snaplines",        &g_cfg.espSnapline);
            ImGui::Checkbox("skeleton",         &g_cfg.espSkeleton);
            ImGui::Checkbox("enemies only",     &g_cfg.espEnemyOnly);
            ImGui::SliderFloat("max distance",  &g_cfg.maxDistance, 100.f, 9000.f, "%.0f u");
            ImGui::ColorEdit4("enemy color",    (float*)&g_cfg.colEnemy);
            ImGui::ColorEdit4("team color",     (float*)&g_cfg.colTeam);
            ImGui::ColorEdit4("text color",     (float*)&g_cfg.colText);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("aim")) {
            ImGui::Checkbox("enabled",          &g_cfg.aimEnabled);
            ImGui::SliderFloat("fov",           &g_cfg.aimFov,    0.5f, 30.f, "%.1f°");
            ImGui::SliderFloat("smoothing",     &g_cfg.aimSmooth, 1.0f, 20.f, "%.2f");
            ImGui::Combo("bone",                &g_cfg.aimBone,   kBones, IM_ARRAYSIZE(kBones));
            int ki = keyToIndex(g_cfg.aimKey);
            if (ImGui::Combo("hotkey", &ki, kAimKeys, IM_ARRAYSIZE(kAimKeys))) {
                g_cfg.aimKey = kAimKeyVK[ki];
            }
            ImGui::Checkbox("team safe",        &g_cfg.aimTeamSafe);
            ImGui::Checkbox("draw fov circle",  &g_cfg.aimDrawFov);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("misc")) {
            ImGui::Checkbox("static crosshair", &g_cfg.crosshair);
            ImGui::Separator();
            ImGui::TextDisabled("menu toggle:  INSERT");
            ImGui::TextDisabled("quit:         END");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("about")) {
            ImGui::Text("madcheats.cc  external");
            ImGui::Text("dx11 overlay, read-only memory");
            ImGui::Separator();
            ImGui::TextWrapped("if offsets drift after a game patch, rebuild with a fresh "
                               "cs2-dumper pass into src/offsets.hpp.");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void DrawCrosshair(int sw, int sh) {
    if (!g_cfg.crosshair) return;
    auto* dl = ImGui::GetBackgroundDrawList();
    float cx = sw * 0.5f, cy = sh * 0.5f;
    ImU32 c  = IM_COL32(0, 255, 120, 220);
    ImU32 sh_c = IM_COL32(0, 0, 0, 220);
    dl->AddLine(ImVec2(cx-7, cy),   ImVec2(cx+7, cy),   sh_c, 3.f);
    dl->AddLine(ImVec2(cx,   cy-7), ImVec2(cx,   cy+7), sh_c, 3.f);
    dl->AddLine(ImVec2(cx-6, cy),   ImVec2(cx+6, cy),   c,    1.f);
    dl->AddLine(ImVec2(cx,   cy-6), ImVec2(cx,   cy+6), c,    1.f);
}

void DrawFovCircle(int sw, int sh) {
    if (!g_cfg.aimDrawFov || !g_cfg.aimEnabled) return;
    auto* dl = ImGui::GetBackgroundDrawList();
    float fovPx = (g_cfg.aimFov / 90.f) * (sw * 0.5f);
    dl->AddCircle(ImVec2(sw * 0.5f, sh * 0.5f), fovPx,
                  IM_COL32(255, 80, 150, 180), 64, 1.2f);
}
