#pragma once
#include <cstdint>

// cs2 offsets. values track a2x-software/cs2-dumper public layout (late 2025 → 2026).
// if a game update breaks things, pull a fresh dump and refresh the two blocks below;
// the entity-list walker in sdk.hpp doesn't depend on these numbers.
namespace off {

    // ---------- client.dll RVAs ----------
    namespace client {
        inline constexpr std::uintptr_t dwEntityList         = 0x19E0CE0;
        inline constexpr std::uintptr_t dwLocalPlayerPawn    = 0x1A2D380;
        inline constexpr std::uintptr_t dwLocalPlayerController = 0x1A29B18;
        inline constexpr std::uintptr_t dwViewMatrix         = 0x1E42DF0;
        inline constexpr std::uintptr_t dwViewAngles         = 0x1E461E0;
        inline constexpr std::uintptr_t dwGlobalVars         = 0x1812F08;
        inline constexpr std::uintptr_t dwGameRules          = 0x1A2AEB8;
        inline constexpr std::uintptr_t dwGameEntitySystem   = 0x1E5BF68;
        inline constexpr std::uintptr_t dwGameEntitySystem_highestEntityIndex = 0x1510;
        inline constexpr std::uintptr_t dwPlantedC4          = 0x1A3CB18;
    }

    // ---------- schema field offsets ----------
    // C_BaseEntity
    inline constexpr std::uintptr_t m_iHealth              = 0x344;
    inline constexpr std::uintptr_t m_iTeamNum             = 0x3E3;
    inline constexpr std::uintptr_t m_lifeState            = 0x338;
    inline constexpr std::uintptr_t m_pGameSceneNode       = 0x310;
    inline constexpr std::uintptr_t m_hController          = 0x7D8;

    // C_BasePlayerPawn / C_CSPlayerPawn
    inline constexpr std::uintptr_t m_vOldOrigin           = 0x1324;
    inline constexpr std::uintptr_t m_pClippingWeapon      = 0x13A0;
    inline constexpr std::uintptr_t m_iszPlayerName        = 0x640;
    inline constexpr std::uintptr_t m_bSpottedByMask       = 0x1638;
    inline constexpr std::uintptr_t m_fFlags               = 0x3DC;
    inline constexpr std::uintptr_t m_vecViewOffset        = 0xCB0;
    inline constexpr std::uintptr_t m_angEyeAngles         = 0x1518;

    // C_CSPlayerController
    inline constexpr std::uintptr_t ctrl_m_sSanitizedPlayerName = 0x640;

    // CSkeletonInstance / CGameSceneNode
    inline constexpr std::uintptr_t m_modelState           = 0x160;
    inline constexpr std::uintptr_t boneArray              = 0x80;    // modelState + boneArray
    inline constexpr std::size_t    boneStride             = 32;      // pos(12) + quat(16) + scale(4) packed

    // bone indices commonly good for cs2 player skeletons
    namespace bone {
        inline constexpr std::uint32_t HEAD        = 6;
        inline constexpr std::uint32_t NECK        = 5;
        inline constexpr std::uint32_t UPPER_CHEST = 4;
        inline constexpr std::uint32_t CHEST       = 3;
        inline constexpr std::uint32_t PELVIS      = 0;
        inline constexpr std::uint32_t L_SHOULDER  = 8;
        inline constexpr std::uint32_t R_SHOULDER  = 13;
    }
}
