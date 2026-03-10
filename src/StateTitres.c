// titles state - scrolls through the final titres, then waits forever

#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Coroutines.h"

#include "GameGlobals.h"

IMPORT_MAP(titres);

NORETURN void TitresLogic(void * custom_data) BANKED {
	(void)custom_data;
	INT16 map_height;
	// set up CrossZGB scrolling parameters
	SPRITES_8x16;
	MAP_OVERLAP_SPR;
	DISABLE_SCROLL_CLAMPING;
	// play outro music
	load_music(MUSIC_OUTRO);
	// destroy all sprites
	SpriteManagerReset();
	// initialize background
	InitScroll(BANK(titres), &titres, NULL, NULL);
	// compensate hidden column on SMS so background is centered
	CompensateScroll();
	// calculate the scroll length for the titres in pixels
	GetMapSize(BANK(titres), &titres, NULL, &map_height);
	map_height -= SCREEN_TILES_H;
	map_height <<= 3;
	// process once to exit the state INIT()
	YIELD;

	// wait 5 seconds
	DELAY(SECONDS(5));

	// scroll titres to the end, one pixel each 8 frames
	for (; (map_height); --map_height) {
		MoveScroll(scroll_x, scroll_y + 1);
		DELAY(8);
	}

	// show RetroSouls logo
#ifdef MASTERSYSTEM
	SpriteManagerAdd(SpriteRetrosouls, 196, scroll_y + 128);
#else
	SpriteManagerAdd(SpriteRetrosouls, 124, scroll_y + 96);
#endif

	// wait forever
	for (;; YIELD);
}

void TitresLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	// titres state logic finalizer,
	MAP_OVERLAP_BKG;
	SPRITES_8x8;
}

STATE_COROUTINE(TitresLogic, TitresLogicFinalizer)
