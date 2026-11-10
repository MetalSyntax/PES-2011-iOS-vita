# Plan de Port — PES 2011 (iOS → PS Vita)

## 0. Contexto

- **Origen:** `PES2011_v1.3.0_os30.ipa` (`com.konami-europe.pes2011`, v1.3.0, MinimumOS 3.0, SDK 3.2, universal iPhone/iPad).
- **TITLEID:** `PSVPES11I`, slug `pes2011ios` (convive con el port Android `PES2011PS` / `pes2011`).
- **Por qué iOS y no Android:** el APK es Airplay SDK 4.3.4 (Marmalade): el juego real es un `.s3e`
  comprimido que se carga y reloca en runtime, con SQLite embebido; quedó bloqueado en Bug 011
  (ver `../Incompletes/PES-2011-vita/port_progress.md`). El IPA es código nativo enlazado de forma normal,
  con los assets sueltos en el bundle, y la familia de ports iOS (HAWX, Ferrari GT, Asphalt 4, …)
  ya tiene runtime probado en consola.

## 1. Binario (`game_app/PES2011`)

- Mach-O `MH_EXECUTE` armv6, delgado, **`cryptid 0`** (desencriptado), no PIE. Entry `0x33c0`, imagen `[0x1000, 0xa37000)`.
- `ios_prep.py`: 52664 palabras de rebase (modo clásico sin relocs, heurístico), 630 símbolos importados
  en 1021 slots, cobertura de `__text` 95.4 %, 12498 palabras dudosas sin rebasear (mirar `report.txt`
  si aparece un puntero < 0x02000000).
- C++: solo 4 imports de libstdc++, todos resueltos (`source/cxx_imports.c`).
- Frameworks: Foundation/CoreFoundation, UIKit, OpenGLES, QuartzCore, AudioToolbox/CoreAudio,
  MediaPlayer, GameKit, StoreKit, iAd, MessageUI, CoreLocation, AddressBook, SystemConfiguration, CFNetwork.
- **GLES:** importa tanto el pipeline fijo (GLES1: `glMatrixMode`, `glTexEnvf`, `glVertexPointer`) como
  GLES2 (`glCreateShader`, `glUseProgram`); falta confirmar cuál elige según `EAGLContext initWithAPI:`.

## 2. Assets (sueltos en el bundle)

- `*.group.bin` (formato propio de Konami): `menu/` (145), `hud/`, `body/`, `stadium/`, `ball/`, `sound/`.
- `database/main.bin` y `save.bin`; `string/{common,en,fr,ge,it,sp}.bin`.
- `scoreloop/` (SDK online de Scoreloop: stubear/omitir).
- `MainWindow.nib` (convertido a `nib.txt` por `stage_data.sh`).
- El audio está dentro de `sound/*.group.bin` (no hay .caf/.m4a sueltos): habrá que ver si usa AudioToolbox, OpenAL o un mezclador propio sobre RemoteIO.

## 3. Checklist

- [x] Scaffold iOS (runtime de HAWX-vita), `port_config.h`, staging y build OK.
- [ ] Corrida #1 en consola: llegar a `UIApplicationMain` → `applicationDidFinishLaunching` sin crash.
- [ ] Contexto GL + primer frame (confirmar GLES1 vs GLES2 y orientación/`PORT_ROTATION`).
- [ ] Carga de `database/*.bin` y `Documents/` (partidas).
- [ ] Audio.
- [ ] Controles (`controls.txt`, toques ↔ botones).
- [ ] Stubs para Scoreloop / GameKit / StoreKit / iAd.
