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

const import_entry patch_imports[] = {
    F(malloc, p_malloc), F(calloc, p_calloc), F(realloc, p_realloc),
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

void patch_init(void) {
    // datos s3e en claro en el binario de staging (0xa1bbd4 = 0x6ff4c rebaseado). Con el PES2011 original
    // (cifrado) en la Vita se llama a basura en los constructores (corrida #2: pc 0xeefd6860 desde 0x240b18)
    if (*(uint32_t *)(0xa1bbd4 + MACHO_SLIDE) != 0x6ff4c + MACHO_SLIDE)
        fatal_error("game_app/" PORT_EXEC " sin descifrar: tools/stage_data.sh y volver a subirlo con tools/deploy_data.py");
    prep_orig = hook_keep_arm(0x275c2c, (void *)p_sqlite3_prepare_v2);
    macho_hook_addr(0xb130, (void *)p_s3eDecodeData);
}
