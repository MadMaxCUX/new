#pragma once
#include "process.hpp"
#include "offsets.hpp"
#include "math.hpp"
#include <array>
#include <optional>
#include <string>

struct Player {
    std::uintptr_t pawn       = 0;
    std::uintptr_t controller = 0;
    int    health             = 0;
    int    team               = 0;
    bool   alive              = false;
    bool   dormant            = false;
    Vec3   origin{};
    Vec3   eye{};
    Vec3   head{};
    std::array<Vec3, 16> bones{};
    char   name[64]{};
};

class World {
public:
    void bind(Process* p, std::uintptr_t clientBase) {
        p_ = p;
        client_ = clientBase;
    }

    ViewMatrix viewMatrix() const {
        return p_->read<ViewMatrix>(client_ + off::client::dwViewMatrix);
    }

    std::uintptr_t localPawnAddr() const {
        return p_->read<std::uintptr_t>(client_ + off::client::dwLocalPlayerPawn);
    }

    int localTeam() const {
        auto pawn = localPawnAddr();
        if (!pawn) return 0;
        return p_->read<std::uint8_t>(pawn + off::m_iTeamNum);
    }

    Vec3 localEye() const {
        auto pawn = localPawnAddr();
        if (!pawn) return {};
        Vec3 origin = p_->read<Vec3>(pawn + off::m_vOldOrigin);
        Vec3 viewOff = p_->read<Vec3>(pawn + off::m_vecViewOffset);
        return origin + viewOff;
    }

    // entity list walk:  list = *(client + dwEntityList)
    // page = *(list + 0x10 + 8*(idx >> 9))
    // ent  = *(page + 120*(idx & 0x1FF))
    std::uintptr_t entity(int idx) const {
        auto list = p_->read<std::uintptr_t>(client_ + off::client::dwEntityList);
        if (!list) return 0;
        auto page = p_->read<std::uintptr_t>(list + 0x10 + 8 * (idx >> 9));
        if (!page) return 0;
        return p_->read<std::uintptr_t>(page + 120 * (idx & 0x1FF));
    }

    int highestIndex() const {
        auto sys = p_->read<std::uintptr_t>(client_ + off::client::dwGameEntitySystem);
        if (!sys) return 64;
        int hi = p_->read<int>(sys + off::client::dwGameEntitySystem_highestEntityIndex);
        if (hi <= 0 || hi > 1024) return 64;
        return hi;
    }

    std::optional<Player> readPlayer(int idx) const {
        std::uintptr_t pawn = entity(idx);
        if (!pawn) return std::nullopt;

        Player pl{};
        pl.pawn   = pawn;
        pl.health = p_->read<int>(pawn + off::m_iHealth);
        pl.team   = p_->read<std::uint8_t>(pawn + off::m_iTeamNum);
        pl.alive  = p_->read<std::uint8_t>(pawn + off::m_lifeState) == 0 && pl.health > 0;
        if (pl.team != 2 && pl.team != 3) return std::nullopt;
        if (pl.health > 100 || pl.health < 0) return std::nullopt;

        pl.origin = p_->read<Vec3>(pawn + off::m_vOldOrigin);
        if (!pl.origin.valid()) return std::nullopt;
        Vec3 viewOff = p_->read<Vec3>(pawn + off::m_vecViewOffset);
        pl.eye = pl.origin + viewOff;

        auto scene = p_->read<std::uintptr_t>(pawn + off::m_pGameSceneNode);
        if (scene) {
            auto boneBase = scene + off::boneArray;
            for (int i = 0; i < (int)pl.bones.size(); ++i) {
                pl.bones[i] = p_->read<Vec3>(boneBase + i * off::boneStride);
            }
            pl.head = pl.bones[off::bone::HEAD];
        } else {
            pl.head = pl.eye;
        }

        auto ctrlHandle = p_->read<std::uint32_t>(pawn + off::m_hController);
        if (ctrlHandle && ctrlHandle != 0xFFFFFFFFu) {
            int ctrlIdx = ctrlHandle & 0x7FFF;
            pl.controller = entity(ctrlIdx);
            if (pl.controller) {
                auto namePtr = p_->read<std::uintptr_t>(pl.controller + off::ctrl_m_sSanitizedPlayerName);
                if (namePtr) {
                    auto s = p_->readString(namePtr, sizeof(pl.name) - 1);
                    std::snprintf(pl.name, sizeof pl.name, "%s", s.c_str());
                }
            }
        }
        if (pl.name[0] == '\0') std::snprintf(pl.name, sizeof pl.name, "player");

        return pl;
    }

private:
    Process*       p_      = nullptr;
    std::uintptr_t client_ = 0;
};
