#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Coroutines.h"

IMPORT_MAP(titres);

void TitresLogic(void * custom_data) BANKED {
	(void)custom_data;
	INT16 map_height;
	// set up CrossZGB scrolling parameters
	MAP_OVERLAP_SPR;
	clamp_enabled = FALSE;
	// destroy all sprites
	SpriteManagerReset();
	Sprite * retrosouls = SpriteManagerAdd(SpriteRetrosouls, DEVICE_SCREEN_PX_WIDTH + 16, 0);
	// initialize background with collisions (skip the very first tile (19), which is only for the player)
	InitScroll(BANK(titres), &titres, NULL, NULL);
#ifdef MASTERSYSTEM
	MoveScroll(8, 0);
#endif
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
}

STATE_COROUTINE(BANK(StateTitres), TitresLogic, TitresLogicFinalizer)
