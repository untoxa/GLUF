#include <gbdk/platform.h>

#include "Math.h"
#include "Scroll.h"

#include "GameGlobals.h"

#include "ZGBMain.h"

#include "parallax_tiles1.h"
#include "parallax_tiles2.h"
#include "parallax_tiles3.h"
#include "parallax_tiles4.h"

#ifdef ENABLE_PARALLAX

#if defined(NINTENDO)
#define BPP_SHIFT 1
#else
#define BPP_SHIFT 2
#endif

#define TILE_SIZE_BYTES (8 << BPP_SHIFT)

static const UINT8 parallax_banks[] = { {BANK(parallax_tiles1)}, {BANK(parallax_tiles2)}, {BANK(parallax_tiles3)}, {BANK(parallax_tiles4)} };
static const UINT8 * const parallax_tiles[] = { parallax_tiles1_0, parallax_tiles2_0, parallax_tiles3_0, parallax_tiles4_0 };

extern tilesets_e current_tileset;

UINT8 parallax_enabled;

void process_parallax(INT16 x, INT16 y) {
	static UINT8 __save;
	static INT16 old_x, old_y;
	__save = CURRENT_BANK;

	if ((x == old_x) && (y == old_y)) return;
	old_x = x, old_y = y;

	const UINT8 * ptr;
	UINT16 offset = ((UINT16)(16u - (y & 0x0f)) << BPP_SHIFT) + ((UINT16)(x & 0x0f) << ((3 + BPP_SHIFT) + 3));

	SWITCH_ROM(parallax_banks[current_tileset]);

	ptr = parallax_tiles[current_tileset] + offset;
	set_bkg_native_data(1, 1, ptr);
	ptr += TILE_SIZE_BYTES;
	set_bkg_native_data(((TILE_LAST_VISIBLE + 1) << 1) + 1, 1, ptr);
	ptr += (TILE_SIZE_BYTES * 3);
	set_bkg_native_data(2, 1, ptr);
	ptr += TILE_SIZE_BYTES;
	set_bkg_native_data(((TILE_LAST_VISIBLE + 1) << 1) + 2, 1, ptr);

	SWITCH_ROM(__save);
}

#if defined(NINTENDO)
extern UINT8 vbl_count;
void VBL_isr(void) NONBANKED {
	vbl_count ++;
	move_bkg(scroll_x_vblank + TILE_TO_PX(scroll_offset_x), scroll_y_vblank + TILE_TO_PX(scroll_offset_y));
	// this happens inside the VBlank interrupt
	if (parallax_enabled) process_parallax(scroll_x_vblank >> 1, scroll_y_vblank >> 1);
}
#else
void SyncVBlank(void) NONBANKED {
	vsync();
	// this happens after main thread returns from the GBDK-2020 vsync() in the main loop
	if (parallax_enabled) process_parallax(scroll_x_vblank >> 1, scroll_y_vblank >> 1);
}
#endif

#endif