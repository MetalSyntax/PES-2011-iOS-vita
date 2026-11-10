# Registro de Progreso — PES 2011 iOS (PS Vita)

## Fase 1: Scaffold iOS (2026-11-10)

El port Android sigue en `../Incompletes/PES-2011-vita` (bloqueado en Bug 011, `.s3e` de Airplay con SQLite embebido). Se arranca desde `PES2011_v1.3.0_os30.ipa`: Mach-O armv6 desencriptado (`cryptid 0`), código nativo y assets sueltos.

- [x] Se copió el runtime iOS de **HAWX-vita** (último port iOS commiteado y publicado) con su `tools/sync_runtime.sh`. `vendor/vitaGL` (SOFTFP_ABI=1) incluida.
- [x] `source/port_config.h`: slug `pes2011ios`, exec `PES2011`, rotación 1, iOS 3.1.3 / iPhone2,1. `patch.c` vacío.
- [x] `tools/stage_data.sh`: `ux0_data/pes2011ios/` con `game_app/`, `rebase.bin` (52664 palabras), `binds.bin` (630 símbolos), `nib.txt`. Carpeta propia `PES-2011-iOS-vita` (como The-Sims-3-iOS-vita): slug `pes2011ios`, TITLEID `PSVPES11I`, no pisa al port Android.
- [x] `tools/gen_cxx.py`: 4 imports C++, todos resueltos.
- [x] Build Debug OK: `build/eboot.bin` (1.4 MB), `build/pes2011ios.vpk`.

## PENDIENTE EN CONSOLA (corrida iOS #1)

1. Subir los datos: `python3 -I tools/deploy_data.py` (sube `ux0_data/pes2011ios/` → `ux0:data/pes2011ios/`).
2. Instalar `build/pes2011ios.vpk` (TITLEID `PSVPES11I`, convive con el Android).
3. Abrir el juego, traer logs y dump con `python3 -I tools/deploy_data.py --logs`. Decir si crasheó o quedó congelado.
4. En un crash: PC − 0x98000000 = dirección en el Mach-O; dirección de datos < 0x02000000 = rebase faltante (buscar en `ux0_data/pes2011ios/report.txt`). Llamadas a selectores/símbolos no implementados salen en el log como `unimpl`.
