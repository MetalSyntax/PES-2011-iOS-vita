// Parches de PES 2011 (iOS, Airplay 4.3.15: runtime en __text, app .s3e embebida en s3e_code/s3e_data).
// Cada parche lleva su evidencia (log/dump + desensamblado) y su dirección del Mach-O.
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>

#include "imports.h"
#include "log.h"
#include "macho.h"
#include "patch.h"
#include "port_config.h"

// Palabras de la pila que apuntan a __TEXT del juego (backtrace aproximado, sin slide).
static void log_guest_stack(int max) {
    uint32_t *sp = (uint32_t *)__builtin_frame_address(0), lo, size;
    if (macho_section("__TEXT", "__text", &lo, &size) < 0)
        return;
    for (int i = 0, k = 0; i < 512 && k < max; i++)
        if (sp[i] >= lo && sp[i] < lo + size)
            l_error("  pila[%d] = 0x%x", i, (unsigned)(sp[i] - MACHO_SLIDE)), k++;
}

// Airplay marca "sin memoria" cuando s3eMallocOS (= malloc) devuelve NULL.
static void *p_malloc(size_t n) {
    void *r = malloc(n);
    if (!r && n) {
        l_error("malloc(%u) = NULL", (unsigned)n);
        log_guest_stack(8);
    }
    return r;
}
static void *p_calloc(size_t a, size_t b) {
    void *r = calloc(a, b);
    if (!r && a && b) {
        l_error("calloc(%u, %u) = NULL", (unsigned)a, (unsigned)b);
        log_guest_stack(8);
    }
    return r;
}
static void *p_realloc(void *o, size_t n) {
    void *r = realloc(o, n);
    if (!r && n) {
        l_error("realloc(%u) = NULL", (unsigned)n);
        log_guest_stack(8);
    }
    return r;
}

// zlib de vitasdk (z_stream = 56 bytes en Darwin 32 bits y newlib). Corrida #5: menuLoad/menuLoadPES (botón
// "siguiente" tras elegir controles) llama inflate y, sin él, la carga no termina y no se presentan más frames.
void inflate(void), inflateEnd(void), inflateInit2_(void), inflateInit_(void), tzset(void);

const import_entry patch_imports[] = {
    F(malloc, p_malloc), F(calloc, p_calloc), F(realloc, p_realloc),
    D(inflate), D(inflateEnd), D(inflateInit2_), D(inflateInit_), D(tzset),
};
TABLE_END(patch)

// 0xb130 = s3eDecodeData(key): descifra los datos de la app en __S3E_DATA con key = hash de [__text, fin de
// s3e_code) (s3eGetCodeHash vía dladdr). ios_prep.py ya los descifró en el binario de staging (clave 0xd0bd9c35)
// y los hooks cambian el hash: se anula.
static void p_s3eDecodeData(uint32_t key) {}

// Hook ARM que conserva el original: sus 2 primeras instrucciones (sin pc) + salto a vm+8.
static uintptr_t hook_keep_arm(uint32_t vm, void *dst) {
    uint32_t c[4] = {macho_read_text(vm), macho_read_text(vm + 4), 0xE51FF004, vm + 8 + MACHO_SLIDE};
    uintptr_t orig = macho_code(c, 4);
    macho_hook_addr(vm, dst);
    return orig;
}

// log: 0x275c2c = sqlite3_prepare_v2(db, sql, n, &stmt, &tail) (lo llama el wrapper 0x13df98)
static uintptr_t prep_orig;
static uint32_t p_sqlite3_prepare_v2(uint32_t db, const char *sql, uint32_t n, uint32_t *st, uint32_t tail) {
    uint32_t r = call_guest6((void *)prep_orig, db, (uint32_t)sql, n, (uint32_t)st, tail, 0);
    if (r || !*st) // 11 + "malformed database schema" = tablas del parser rebaseadas (ios_prep.py, zona SQLite)
        l_error("sqlite3_prepare_v2(%.120s) = %u", sql ? sql : "(null)", r);
    return r;
}

// Corrida #4 (dump 0x0000442d5f, stop 0x10006): el hilo de carga corría código de la app con SP = 0x81d7c678 (heap)
// y su pila real es 0x94b01000-0x94d01000; la Vita mata un hilo con SP fuera de su pila. Es el StackSwitch de s3e:
// 0x8be0 crea con 0x1fa54(entry, size = icf StackSize, arg, buf) una fibra para la app y, con buf = 0, la pila sale
// del heap de s3e (0x2b6f8). Apagar StackSwitch no sirve en Airplay 4.3 (los trampolines 0x35690 usan igual la pila
// guardada en 0xa0daf0). Se le da un buf dentro de la pila real del hilo de carga (2 MB, aquí SP aún está arriba):
// 512 KB bajo el SP actual quedan para el lado sistema (s3e vuelve a la pila original en cada llamada al SO).
#define FIBER_SYS_RESERVE (512 * 1024)
#define FIBER_MIN_STACK (256 * 1024)
static uintptr_t fiber_orig;
static uint32_t p_s3e_fiber_create(uint32_t entry, uint32_t size, uint32_t arg, uint32_t buf) {
    if (!buf) {
        if (size < FIBER_MIN_STACK)
            size = FIBER_MIN_STACK;
        // ponytail: asume que el hilo creador tiene >= 512 KB + size libres bajo SP (el de carga pide 2 MB)
        buf = (((uint32_t)__builtin_frame_address(0) - FIBER_SYS_RESERVE - size) & ~0xfffu);
        l_info("s3e: pila de la fibra %u KB en la pila del hilo (0x%08x)", size / 1024, buf);
    }
    return call_guest((void *)fiber_orig, entry, size, arg, buf);
}

void patch_init(void) {
    fiber_orig = hook_keep_arm(0x1fa54, (void *)p_s3e_fiber_create);
    // datos s3e en claro en el binario de staging (0xa1bbd4 = 0x6ff4c rebaseado). Con el PES2011 original
    // (cifrado) en la Vita se llama a basura en los constructores (corrida #2: pc 0xeefd6860 desde 0x240b18)
    if (*(uint32_t *)(0xa1bbd4 + MACHO_SLIDE) != 0x6ff4c + MACHO_SLIDE)
        fatal_error("game_app/" PORT_EXEC " sin descifrar: tools/stage_data.sh y volver a subirlo con tools/deploy_data.py");
    prep_orig = hook_keep_arm(0x275c2c, (void *)p_sqlite3_prepare_v2);
    macho_hook_addr(0xb130, (void *)p_s3eDecodeData);
}
