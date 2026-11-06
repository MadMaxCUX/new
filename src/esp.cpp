#include "esp.hpp"
#include "menu.hpp"
#include <imgui.h>
#include <cmath>
#include <algorithm>

static ImU32 col(ImVec4 c) { return ImGui::ColorConvertFloat4ToU32(c); }

void DrawESP(const World& world, const ViewMatrix& vm, int myTeam,
             const Vec3& localEye, int sw, int sh) {
    if (!g_cfg.espEnabled) return;
    auto* dl = ImGui::GetBackgroundDrawList();

    int hi = world.highestIndex();
    hi = std::clamp(hi, 32, 128);

    for (int i = 1; i <= hi; ++i) {
        auto op = world.readPlayer(i);
        if (!op) continue;
        const Player& p = *op;
        if (!p.alive) continue;
        if (g_cfg.espEnemyOnly && p.team == myTeam) continue;
        if (p.team == 0 || p.team == 1) continue;

        float dist = (p.origin - localEye).length();
        if (dist > g_cfg.maxDistance) continue;

        Vec3 head = p.head;
        head.z += 10.f;
        Vec3 feet = p.origin;

        Vec2 sFeet{}, sHead{};
        if (!WorldToScreen(vm, feet, sw, sh, sFeet)) continue;
        if (!WorldToScreen(vm, head, sw, sh, sHead)) continue;

        float height = std::fabs(sFeet.y - sHead.y);
        if (height < 8.f) continue;
        float width  = height * 0.42f;
        float left   = sFeet.x - width * 0.5f;
        float right  = sFeet.x + width * 0.5f;
        float top    = sHead.y;
        float bottom = sFeet.y;

        bool enemy = (p.team != myTeam);
        ImU32 c    = col(enemy ? g_cfg.colEnemy : g_cfg.colTeam);
        ImU32 ct   = col(g_cfg.colText);
        ImU32 sh_c = IM_COL32(0, 0, 0, 220);

        if (g_cfg.espBox) {
            dl->AddRect(ImVec2(left - 1, top - 1), ImVec2(right + 1, bottom + 1),
                        sh_c, 0.f, 0, g_cfg.boxThickness + 1.5f);
            dl->AddRect(ImVec2(left, top), ImVec2(right, bottom),
                        c, 0.f, 0, g_cfg.boxThickness);
        }

        if (g_cfg.espHealthBar) {
            float h  = std::clamp(p.health / 100.f, 0.f, 1.f);
            float bx = left - 6.f;
            dl->AddRectFilled(ImVec2(bx - 1, top - 1), ImVec2(bx + 4, bottom + 1),
                              IM_COL32(0, 0, 0, 220));
            dl->AddRectFilled(ImVec2(bx, top + (bottom - top) * (1.f - h)),
                              ImVec2(bx + 3, bottom),
                              IM_COL32((int)(255 * (1 - h)), (int)(255 * h), 40, 255));
        }

        float textY = bottom + 2.f;
        if (g_cfg.espName && p.name[0]) {
            ImVec2 ts = ImGui::CalcTextSize(p.name);
            float  x  = sFeet.x - ts.x * 0.5f;
            dl->AddText(ImVec2(x + 1, textY + 1), sh_c, p.name);
            dl->AddText(ImVec2(x,     textY    ), ct,   p.name);
            textY += ts.y;
        }
        if (g_cfg.espHealthText) {
            char buf[16];
            std::snprintf(buf, sizeof buf, "%d hp", p.health);
            ImVec2 ts = ImGui::CalcTextSize(buf);
            float  x  = sFeet.x - ts.x * 0.5f;
            dl->AddText(ImVec2(x + 1, textY + 1), sh_c, buf);
            dl->AddText(ImVec2(x,     textY    ), ct,   buf);
            textY += ts.y;
        }
        if (g_cfg.espDistance) {
            char buf[24];
            std::snprintf(buf, sizeof buf, "%.0fm", dist / 100.f);
            ImVec2 ts = ImGui::CalcTextSize(buf);
            float  x  = sFeet.x - ts.x * 0.5f;
            dl->AddText(ImVec2(x + 1, textY + 1), sh_c, buf);
            dl->AddText(ImVec2(x,     textY    ), ct,   buf);
            textY += ts.y;
        }

        if (g_cfg.espSnapline) {
            dl->AddLine(ImVec2(sw * 0.5f, (float)sh),
                        ImVec2(sFeet.x, sFeet.y), c, 1.f);
        }

        if (g_cfg.espSkeleton) {
            const int links[][2] = {
                {0, 3}, {3, 4}, {4, 5}, {5, 6},
                {4, 8}, {8, 9}, {9, 10},
                {4, 12}, {12, 13}, {13, 14},
                {0, 15}
            };
            for (auto& L : links) {
                Vec2 a{}, b{};
                if (WorldToScreen(vm, p.bones[L[0]], sw, sh, a) &&
                    WorldToScreen(vm, p.bones[L[1]], sw, sh, b)) {
                    dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), c, 1.1f);
                }
            }
        }
    }
}
