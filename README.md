# madcheats.cc  external

Standalone Windows overlay for Counter-Strike 2 with ImGui menu, ESP and aimbot.
Read-only memory access (`ReadProcessMemory`) + external `SendInput` mouse steering.
No DLL injection, no hooks into the game, no writes into the game.

```
cs2.exe  <─ RPM ─   madcheats.cc.exe  ─ SendInput ─>  OS mouse
                           │
                   borderless transparent
                   DX11 overlay (ImGui)
```

## quick start

Prebuilt binary lands in GitHub Actions → latest green `build` run → artifact
**madcheats-external** → `madcheats.cc.exe`.

To build it yourself, Windows 10/11 + Visual Studio 2022 + CMake 3.20+:

```powershell
# clone this repo, then:
powershell -ExecutionPolicy Bypass -File scripts/build.ps1
# result:  build/bin/madcheats.cc.exe
```

Run it **as administrator** so it can open `cs2.exe`:

1. Launch CS2 and reach the main menu (so `client.dll` is mapped).
2. Set CS2 to **Fullscreen Windowed** (true fullscreen hides the overlay).
3. Right-click `madcheats.cc.exe` → Run as administrator.
4. Press `INSERT` to toggle the menu, `END` to quit.
5. Hold `Mouse5` (configurable) to engage aimbot.

## offsets

`src/offsets.hpp` tracks the public [a2x-software/cs2-dumper]
layout. After a game patch, refresh those numbers from any current dump
and rebuild.

The entity-list walker in `src/sdk.hpp` uses the stable
`list + 0x10 + 8*(i>>9)` → `*(page + 120*(i&0x1FF))` shape and is
independent of patch-level offsets.

## layout

```
src/
  main.cpp        attach + overlay loop + aim thread + hotkeys
  overlay.*       DX11 transparent top-most layered window + ImGui backend
  process.*       OpenProcess, module enumeration, RPM helpers
  sdk.hpp         entity-list walk, Player struct, bone reads
  offsets.hpp     patch-specific RVAs + schema offsets
  esp.cpp         box / name / health / distance / snaplines / skeleton
  aim.cpp         fov-cone nearest-target + smoothed SendInput nudge
  menu.cpp        ImGui tabbed menu (esp / aim / misc / about)
  math.hpp        world-to-screen + vec types
third_party/imgui fetched by scripts/fetch_imgui.ps1 (v1.90.9)
```

## hotkeys (default)

| key           | action              |
|---------------|---------------------|
| `INSERT`      | toggle menu         |
| `END`         | quit                |
| `Mouse5`      | aimbot hold         |

## notes

- Overlay follows the CS2 window position / size automatically.
- VAC does not scan user-mode processes; third-party anticheats on
  community servers (FaceIt / ESEA) do — this tool will trip them.
- Only use on your own account, on your own machine.
