# Registro de Progreso — PES 2011 iOS (PS Vita)

## Fase 1: Scaffold iOS (2026-11-10)

El port Android sigue en `../Incompletes/PES-2011-vita` (bloqueado en Bug 011, `.s3e` de Airplay con SQLite embebido). Se arranca desde `PES2011_v1.3.0_os30.ipa`: Mach-O armv6 desencriptado (`cryptid 0`), código nativo y assets sueltos.

- [x] Se copió el runtime iOS de **HAWX-vita** (último port iOS commiteado y publicado) con su `tools/sync_runtime.sh`. `vendor/vitaGL` (SOFTFP_ABI=1) incluida.
- [x] `source/port_config.h`: slug `pes2011ios`, exec `PES2011`, rotación 1, iOS 3.1.3 / iPhone2,1. `patch.c` vacío.
- [x] `tools/stage_data.sh`: `ux0_data/pes2011ios/` con `game_app/`, `rebase.bin` (52664 palabras), `binds.bin` (630 símbolos), `nib.txt`. Carpeta propia `PES-2011-iOS-vita` (como The-Sims-3-iOS-vita): slug `pes2011ios`, TITLEID `PSVPES11I`, no pisa al port Android.
- [x] `tools/gen_cxx.py`: 4 imports C++, todos resueltos.
- [x] Build Debug OK: `build/eboot.bin` (1.4 MB), `build/pes2011ios.vpk`.

## Fase 2: runtime de PES-2012-iOS-vita (2026-11-13)

- [x] `source/` y `tools/` comunes sincronizados desde `../PES-2012-iOS-vita` (`tools/sync_runtime.sh`), + `storekit.c`,
  `hottest.py`, `dump_threads.py`. vitaGL sin `DRAW_SPEEDHACK` (recompilada). `patch.c` sigue vacío.
- [x] `port_config.h`: `DARWIN_INODE64 1` (mismo Marmalade/SDK que PES 2012), `PORT_WATCHDOG 20`, sin `PORT_IAP_UNLOCK`.
- [x] `ios_prep.py`: el filtro de constantes SQLite necesita `__symbolstub1` (layout PES 2012); PES 2011 trae `s3e_code`
  y crasheaba el script → se saltea. rebase.bin: 53468 palabras.

### Corrida #1 (`logs/pes2011ios_001.log`, datos de la fase 1: rebase 52664)
- FATAL `sin símbolo _main y no se encontró la llamada en start (0x000033C0)`: el crt1 de SDK 3.2 no hace `bl main`,
  sino `ldr ip,[pc,#n]; add ip,pc,ip; ldr ip,[ip]; blx ip` con main en `__nl_symbol_ptr` 0xA00020 (= 0x3e1cc).
  → `find_main_from_start` (main.c) reconoce ese patrón.
- `plist no binario (sin soporte XML)`: Info.plist del IPA en XML → `stage_data.sh` convierte los .plist con
  `plutil -convert binary1`.

## Fase 3: hosttest (2026-11-18)

- `hosttest/` copiado de `../Iron-Man-2-iOS-vita` (mismo runtime, sin core externo): loader en Linux ARM32 softfp (Docker armel, imágenes `templerun2-hosttest-*`).
  - `hosttest/run.sh build` y `hosttest/run.sh run [s]`.
  - Logs en `build/hosttest-data/pes2011ios/logs`; capturas PPM en `build/hosttest-data/frames` (`HOSTTEST_FRAME_S`).
- Fakes nuevos en `psp2_extra.c`: `kuKernel*ExceptionHandler`, `sceIoDevctl`, `sceKernelPowerTick`, `glClearColorx`, `glLightxv`, `glLightModelxv`, `glDepthRangex`, `glPolygonOffsetx`, `glAlphaFuncx`, `glFogx`, `glFrustumx`.
- `gl_thread.c`: el contexto EGL va al "loader thread" de Airplay (0x3E5F8, pila 2048 KB), no al de 512 KB.
- `gles.c`: `glPolygonOffsetx` enlazado (vitaGL lo tiene).
- Resultado: llega a la pantalla de título ("PRO EVOLUTION SOCCER 2011 / TAP ANYWHERE") sin crash, ~30 fps, 180-400 s estable.
- `rebase.bin`: 51512 palabras. Cobertura s3e_code 75.6 %.

### Bugs resueltos en el hosttest (en orden, todos en `tools/ios_prep.py` salvo indicación)
- #H1 `Insufficient memory... Not enough memory to begin loading s3e file`: `malloc(0x98014470)`. `s3eMallocOS_platform` (0x2c050, wrapper ARM de malloc) recibía `ldr r0,=0x11170` rebaseado (0x2b6f4). Binario stripped: se detectan wrappers `push {r7,lr}; add r7,sp,#0; bl <asignador>; pop {r7,pc}` como asignadores (−1 rebase).
- #H2 salto a 0x8cb4 sin slide: callback ARM < 0x10000 pasado como argumento (0x8c24 `ldr r0,=0x8cb4`). Regla lit→text-low: se acepta un inicio exacto de función usado como arg/store/ptr (+62, tablas de funciones que arma Airplay).
- #H3 datos s3e cifrados: en Airplay 4.3 la app vive en la sección `s3e_code` (no `__symbolstub1`). `app_sect()` devuelve `__symbolstub1` o `s3e_code`; `s3e_decrypt` usa [`__text`, fin de `s3e_code`): clave 0xd0bd9c35, 0x89bc bytes. `patch.c` anula `s3eDecodeData` (0xb130). `s3e_code` se escanea como código: −4334 rebases falsos dentro del código de la app, +1467 punteros en `__S3E_DATA`.
- #H4 constructores estáticos de la app: tabla de offsets relativos 0x2d2f18-0x2d33b4 (295) recorrida por el bucle Thumb 0x7176a; sus entradas son raíces (init ARM 0x1eb054 llamaba `__aeabi_vec_ctor` con 0x14b520/0xa316e0 sin slide).
- #H5 veneers de la app `bx pc; nop; ldr ip,[pc]; ldr pc,[pc]; .word destino; .word thunk`: ambas palabras se rebasean siempre (+746; 0x2d94c8 → hoja 0x44b04).
- #H6 función hoja sin prólogo tras un pool, guardada junto a callbacks ya aceptados (0x26760 = 0x2668c): vecino ±4 del mismo pool aceptado → puntero (+3).
- #H7 tabla de 128 thunks en `__data` 0xa06804.. (0x358e0, 0x35908, … cada 0x30): corrida ≥ 4 palabras de datos crecientes hacia `__text` con paso ≤ 0x100.
- #H8 vtables Itanium de la app `[offset_to_top][typeinfo->{vptr,nombre}][entradas]`: las entradas son raíces forzadas del escaneo (antes un método no alcanzado se tomaba por datos: 0x14010c = `adr r1; movs r0,r4` rebaseado → `this` basura) y se rebasean (0x2bf530/0x2bf534, hojas 0x11b524/0x11b51c).
- #H9 funciones hoja del runtime solo alcanzables por las tablas de API de la app (0x2e5cdc → 0x1cf10, literal 0x1cef0 = 0xa002e4 sin rebase): raíces = palabras de datos/app hacia `__text` ARM alineado tras `bx lr`/`pop {pc}`/`b`/pool (1763 raíces).
- #H10 SQLite: todas las prepare devolvían 11, `malformed database schema (sqlite_master) - near ",": syntax error`. La cota inferior de la zona de constantes SQLite pasa a `sqlite_rng[0] − 0x80000` (PES 2012 tenía 0x200000 fijo): −5 rebases de pares u16 del parser (0x2a2978, 0x2a29a0, 0x2a2b1c, 0x2a2f28, 0x2a3444). `patch.c` loguea `sqlite3_prepare_v2` (0x275c2c) cuando falla.
- #H11 CodeScan: un destino que empieza con `push {..,lr}` (Thumb 0xb5xx / ARM stmfd) no se frena como "tabla" (`bl` → 0x1c3258 tomado por datos; rebase en mitad de un `bl` en 0x1c329c): −7.

### Pendiente visto en el hosttest (verificar en consola)
- El texto "© 2010 Konami Digital Entertainment" y "TAP ANYWHERE" sale estirado en vertical sobre el logo (no es `glFrustumx`).
- Texturas PVRTC (0x8C02) no las sube Mesa (en la Vita vitaGL sí).
- Unimpl en el título: `_CFStringCompare`, `_AudioFileOpenWithCallbacks`, `_tzset`.
- Riesgo conocido: 0x2ae9a0 = 0x00070004 (tabla u16 creciente 4, 7, 10, 14…) sigue rebaseado fuera de la zona SQLite.

### Corrida #2 (`logs/pes2011ios_002.log`, dump 0x000a4c30a9)
- Prefetch abort pc=0xeefd6860 en los constructores de la app (lr 0x1cf437, veneer 0x240b18 `ldr r1,=0xa1bbd4; ldr r1,[r1]; bx r1`).
  En staging 0xa1bbd4 = 0x6ff4c (rebaseado); en la Vita, basura: el `game_app/PES2011` de la consola seguía siendo el
  original con los datos s3e cifrados (y el Info.plist en XML). `deploy_data.py` solo subía archivos de tamaño distinto
  y el binario descifrado pesa lo mismo → `PES2011` e `Info.plist` pasan a `ALWAYS`. `patch_init` aborta con un mensaje
  claro si 0xa1bbd4 no está descifrado. Subidos datos y `eboot.bin` (ux0:app/PSVPES11I) por FTP.

### Corrida #3 (`logs/pes2011ios_003.log`, dump 0x0009e1386f)
- Llega al logo de Konami, toque, splash del menú. `_AudioFileOpenWithCallbacks` sin implementar (música MP3 dentro de
  `sound/ambientMenu00.group.bin`, leída por callbacks de s3eFile) y data abort en 0x155e8c (`refcount--` sobre
  0x982bf6d4, que es una vtable en __TEXT).
- Reproducido en el hosttest con __TEXT de solo lectura tras el primer frame (vitagl_fake.c, mprotect): misma pila.
  Causa: 0x2896b8 (`movs r2,r0; movs r0,r1; push {r4,lr}`, destino de bl) rebaseado como "puntero" 0x00080002 a la
  cadena " == " → `movs r0,r1` pasaba a `ldr r0,[sp,#0x20]` y la asignación de un puntero inteligente escribía en basura.
  `ios_prep.py`: una entrada de función decodificada (destino de bl) nunca pasa por la heurística de datos (−4:
  0x1c3258, 0x28950c, 0x28966c, 0x2896b8); destino de bl Thumb con push en las 4 primeras medias palabras no se frena
  como tabla. rebase.bin: 51508.
- `audio.c`: `AudioFileOpenWithCallbacks` copia el archivo con los callbacks del juego y lo decodifica con mpg123
  (vitasdk) como LPCM estéreo 16 bits. `foundation.c`: `CFStringCompare`. hosttest: mpg123 en las imágenes Docker,
  nombre de hilo en el crash.
- Hosttest: llega al menú principal (Exhibition) con música, sin crash en 420 s. Subidos datos y eboot.

## PENDIENTE EN CONSOLA (corrida iOS #4)

1. `tools/stage_data.sh` regenera `ux0_data/` (re-descifra el Mach-O de staging y `rebase.bin`). Subir con `python3 -I tools/deploy_data.py` (sube `ux0_data/pes2011ios/` → `ux0:data/pes2011ios/`; el PES2011 de `game_app/` cambia: datos s3e descifrados).
2. Instalar `build/pes2011ios.vpk` (TITLEID `PSVPES11I`, convive con el Android).
3. Abrir el juego, traer logs y dump con `python3 -I tools/deploy_data.py --logs`. Objetivo: confirmar la pantalla de título en hardware y el texto estirado. Decir si crasheó o quedó congelado.
4. En un crash: PC − 0x98000000 = dirección en el Mach-O; dirección de datos < 0x02000000 = rebase faltante (buscar en `ux0_data/pes2011ios/report.txt`). Llamadas a selectores/símbolos no implementados salen en el log como `unimpl`.
