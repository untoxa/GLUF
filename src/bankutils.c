// various wrappers for the GBDK-2020 functions for working with banked data

#include <gbdk/platform.h>
#include <gbdk/zx0decompress.h>

static UINT8 _save;

void zx0_decompress_banked(const void *sour, void *dest, UINT8 bank) NONBANKED {
	_save = _current_bank;
	SWITCH_ROM(bank);
	zx0_decompress((void *)sour, dest);
	SWITCH_ROM(_save);
}
