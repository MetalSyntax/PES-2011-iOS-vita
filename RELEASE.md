# PES 2011 (PS Vita) — Beta Release v0.1

First public build of the native iOS-to-PS Vita wrapper port of *Pro Evolution Soccer 2011* (iOS v1.3.0). On real hardware the game boots to the title screen, the menus and team selection work, and a friendly match (Exhibition) can be played. The options menu (Select + Start) and the physical controls work on the console. A purple tint on the ball and players during set pieces is still open.

## 📦 Release Package Contents

- **`pes2011ios.vpk`** — the loader (Title ID `PSVPES11I`, version `01.00`), with the LiveArea. No game code or assets.
- **`rebase.bin`** — relocation table for the 1.3.0 `PES2011` executable (51,461 words).
- **`binds.bin`** — import slots for the Apple framework functions the executable uses.
- **`controls.txt`** — default button → touch mapping for the PES 2011 match screen (editable without recompiling).

## 🛠️ What are rebase.bin and binds.bin?

The loader maps the armv6 iOS executable at a fixed slide (`0x98000000`). `rebase.bin` lists every word holding an absolute address that must be moved by that slide, and `binds.bin` lists the slots the executable imports from Apple frameworks. Both are generated from your own executable by `tools/ios_prep.py` (run by `tools/stage_data.sh`); the attached copies match the 1.3.0 IPA.

## 🚀 Player Installation Guide

1. **Requirements**: Enso 3.60/3.65, **kubridge** in `ur0:tai/config.txt` (`*KERNEL` section), and `libshacccg.suprx` in `ur0:data/`.
2. **Game files**: a decrypted *PES 2011* **1.3.0** IPA (`cryptid 0`). With Python 3 and `ffmpeg` (music), clone the repo, extract `Payload/PES2011.app/`, link it as `game_app` and run:
   ```bash
   tools/stage_data.sh
   ```
   This creates `ux0_data/pes2011ios/` with `rebase.bin`, `binds.bin`, `nib.txt`, `nibs/` and the converted music.
3. **Copy** `ux0_data/pes2011ios/` to `ux0:data/pes2011ios/` on the Vita (`game_app/`, `rebase.bin`, `binds.bin`, `nib.txt`, `nibs/`, `controls.txt`).
4. **Install** `pes2011ios.vpk` with VitaShell.
5. **Options**: hold **Select + Start** in game to change the screen size, filter, MSAA and rotation (saved to `options.txt`).

## 🎮 Controls

| Input | Action | Status |
| :---: | :--- | :---: |
| **Touch screen** | Menus and match | ✅ Working |
| **Left Stick** | Virtual joystick (moves the player) | ✅ Working |
| **Cross (✖)** | Button A | ✅ Working |
| **Circle (●) / Square (◼)** | Button B | ✅ Working |
| **Start** | In-game pause | ⚠️ Not confirmed |
| **Select + Start** | Options menu | ✅ Working |

## 📋 Changelog (v0.1)

- **Startup**: the SDK 3.2 start code calls `main` through a pointer load, not a direct call → `main` is located from that pattern.
- **Staging**: the IPA's binary-plist `Info.plist` could not be read → plists are converted to binary during staging.
- **Splash / loading**: the s3e StackSwitch fiber was given no buffer and the splash thread died → the fiber gets a 256 KB buffer inside the loading thread's stack.
- **Menu**: `inflate*` and `tzset` were missing, so the menu load stalled → implemented in `patch.c`.
- **Music**: `AudioFileOpenWithCallbacks` was missing (the menu music is MP3 inside a `.group.bin`) → decoded with mpg123 to PCM.
- **Player models**: sine-table words were rebased as pointers, which corrupted the skinning quaternions → excluded through `tools/rebase_skip.txt`; the spikes are gone on console.
- **Set-piece textures**: the present path read the texture state of the active unit and applied it to unit 0 → each unit is read on its own (the tint is not fully fixed, see Known Issues).
- **Options menu**: Select + Start opens a video options panel (screen size, filter, MSAA, rotation), saved to `options.txt`.
- **Controls**: physical buttons mapped to touches through `controls.txt` (left stick, Cross, Circle/Square, Start).

## ⚠️ Known Issues

- **Purple/red set-piece colours**: during kick-offs and corners the ball and players stay tinted. Not fixed in v0.1.
- **Not verified on console yet**: audio, saves and the Start button.
- **Online features disabled**: StoreKit is stubbed; no in-app purchases.

Installation and build instructions: see [README.md](README.md).
