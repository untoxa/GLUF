#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Coroutines.h"

#include "levels.h"

IMPORT_MAP(titres);

NORETURN void TitresLogic(void * custom_data) BANKED {
	(void)custom_data;
	INT16 map_height;
	// set up CrossZGB scrolling parameters
	SPRITES_8x16;
	MAP_OVERLAP_SPR;
	clamp_enabled = FALSE;
	// destroy all sprites
	SpriteManagerReset();
	Sprite * retrosouls = SpriteManagerAdd(SpriteRetrosouls, DEVICE_SCREEN_PX_WIDTH + 16, 0);
	// initialize background
	InitScroll(BANK(titres), &titres, NULL, NULL);
	// compensate hidden column on SMS so background is centered
	CompensateScroll();
	// calculate the scroll limit for the titres
	GetMapSize(BANK(titres), &titres, NULL, &map_height);
	map_height -= SCREEN_TILES_H;
	YIELD;

	// wait 5 seconds
	for (UINT16 i = 0; i != (60 * 5); ++i, YIELD);
	// scroll titres to the end
	if (map_height > 0) {
		while (map_height--) {
			for (UINT8 i = 0; i != 8; ++i) {
				MoveScroll(scroll_x, scroll_y + 1);
				for (UINT8 j = 0; j != 8; ++j, YIELD);
			}
			if (retrosouls) retrosouls->y = scroll_y;
		}
	}
#ifdef MASTERSYSTEM
	if (retrosouls) retrosouls->x = 196, retrosouls->y += 128;
#else
	if (retrosouls) retrosouls->x = 124, retrosouls->y += 96;
#endif
	// wait forever
	for (;; YIELD);
}

void TitresLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	MAP_OVERLAP_BKG;
	SPRITES_8x8;
}

STATE_COROUTINE(TitresLogic, TitresLogicFinalizer)
