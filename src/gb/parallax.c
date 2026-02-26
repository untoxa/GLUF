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

static const UINT8 parallax_banks[] = { {BANK(parallax_tiles1)}, {BANK(parallax_tiles2)}, {BANK(parallax_tiles3)}, {BANK(parallax_tiles4)} };
static const UINT8 * const parallax_tiles[] = { parallax_tiles1_0, parallax_tiles2_0, parallax_tiles3_0, parallax_tiles4_0 };

extern tilesets_e current_tileset;

void process_parallax(void) {
	static UINT8 __save;
	__save = CURRENT_BANK;

	const UINT8 * ptr;
	UINT16 offset = ((UINT16)(16u - ((scroll_y_vblank >> 1) & 0x0f)) << 1) + ((UINT16)((scroll_x_vblank  >> 1) & 0x0f) << (4 + 3));

	SWITCH_ROM(parallax_banks[current_tileset]);

	ptr = parallax_tiles[current_tileset] + offset;
	set_bkg_data(1, 1, ptr);
	ptr += 16;
	set_bkg_data((16 << 1) + 1, 1, ptr);
	ptr += 16 + 32;
	set_bkg_data(2, 1, ptr);
	ptr += 16;
	set_bkg_data((16 << 1) + 2, 1, ptr);

	SWITCH_ROM(__save);
}

extern UINT8 vbl_count;
void VBL_isr(void) NONBANKED {
	vbl_count ++;
	move_bkg(scroll_x_vblank + TILE_TO_PX(scroll_offset_x), scroll_y_vblank + TILE_TO_PX(scroll_offset_y));

	if (current_state == StateGame) process_parallax();
}

#endif