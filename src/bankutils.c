#include <gbdk/platform.h>
#include <gbdk/zx0decompress.h>
#include <stdint.h>

static uint8_t _save;

void * zx0_decompress_banked(const void *sour, void *dest, uint8_t bank) NONBANKED {
    _save = _current_bank;
    SWITCH_ROM(bank);
    zx0_decompress((void *)sour, dest);
    SWITCH_ROM(_save);
    return dest;
}
