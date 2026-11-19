#pragma once
// Configuración propia de este juego (el resto de source/ es común a los ports iOS de la familia;
// se sincroniza con tools/sync_runtime.sh desde PES-2012-iOS-vita).
#define PORT_NAME "PES 2011"
#define PORT_SLUG "pes2011ios"
#define PORT_EXEC "PES2011"            // CFBundleExecutable
#define PORT_ROTATION 1                // drawable vertical -> 1 landscape-right, 2 landscape-left, 0 vertical
#define PORT_STRETCH 0                 // 0 = conservar aspecto (816x544 con barras), 1 = estirar a 960x544
#define PORT_SYSTEM_VERSION "3.1.3"    // [UIDevice systemVersion] (MinimumOSVersion 3.0, SDK 3.2)
#define PORT_DEVICE_MACHINE "iPhone2,1" // hw.machine (3GS)
#define GLES_POOL_MB 24
#define PORT_ACCEL_FROM_STICK 0        // por confirmar si el juego usa acelerómetro
#define DARWIN_INODE64 1              // dirent/stat de inodo de 64 bits (mismo SDK/Marmalade que PES 2012: _readdir/_stat sin sufijo)
// Sin PORT_IAP_UNLOCK: el binario no tiene productos com.konami.* de StoreKit
#define PORT_AUTOTAP 0                // diagnóstico: toque automático en el centro a los N s (0 = apagado)
#define PORT_DUMP_AT 0                // diagnóstico: dump forzado a los N s de proceso (0 = apagado)
#define PORT_WATCHDOG 20              // s sin girar el run loop -> dump forzado (diagnóstico de cuelgues; 0 = apagado)
