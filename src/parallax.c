// "fake parallax" is achieved by streaming 4 shifted background tiles each VBlank,
// depending on the scroll position; "background" is scrolling twice slower than the
// "foreground" and that produces illusion as if it is "farer" from the viewer

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

extern tilesets_e current_tileset;

static UINT8 parallax_enabled;
static INT16 old_x, old_y;
void process_parallax(INT16 x, INT16 y) NONBANKED {
	static const UINT8 parallax_banks[] = { {BANK(parallax_tiles1)}, {BANK(parallax_tiles2)}, {BANK(parallax_tiles3)}, {BANK(parallax_tiles4)} };
	static const UINT8 * const parallax_tiles[] = { parallax_tiles1_0, parallax_tiles2_0, parallax_tiles3_0, parallax_tiles4_0 };

	static UINT8 __save;

	if ((x == old_x) && (y == old_y)) return;
	old_x = x, old_y = y;

	const UINT8 * ptr;
	UINT16 offset = ((UINT16)(16u - (y & 0x0f)) << BPP_SHIFT) + ((UINT16)(x & 0x0f) << ((3 + BPP_SHIFT) + 3));

	__save = CURRENT_BANK;
	SWITCH_ROM(parallax_banks[current_tileset]);

	ptr = parallax_tiles[current_tileset] + offset;
	set_bkg_native_data(ID_TO_TILE(TILE_EMPTY), 2, ptr);
	ptr += (TILE_SIZE_BYTES * 4);
	set_bkg_native_data(ID_TO_TILE(TILE_EMPTY) + 2, 2, ptr);

	SWITCH_ROM(__save);
}

// override the standard CrossZGB VBlank handler
extern UINT8 vbl_count;
void VBL_isr(void) NONBANKED {
	vbl_count ++;
#if defined(NINTENDO)
	move_bkg(scroll_x_vblank + TILE_TO_PX(scroll_offset_x), scroll_y_vblank + TILE_TO_PX(scroll_offset_y));
#elif defined(SEGA)
	// do nothing if data loading to VDP was interrupted
	if (_shadow_OAM_OFF) return;
	// update the scroll position
	__WRITE_VDP_REG_UNSAFE(VDP_RSCX, -(scroll_x_vblank + TILE_TO_PX(scroll_offset_x)));
	__WRITE_VDP_REG_UNSAFE(VDP_RSCY, (((UINT16)(scroll_y_vblank + TILE_TO_PX(scroll_offset_y))) % TILE_TO_PX(DEVICE_SCREEN_BUFFER_HEIGHT)));
#endif
	// load the fake parallax tiles into VRAM
	if (parallax_enabled) process_parallax(scroll_x_vblank >> 1, scroll_y_vblank >> 1);
}

void enable_parallax(void) {
	old_x = old_y = 0x7fff;
	parallax_enabled = TRUE;
}

void disable_parallax(void) {
	parallax_enabled = FALSE;
}

#else

void enable_parallax(void) {
	return;
}

void disable_parallax(void) {
	return;
}

#endif