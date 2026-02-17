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
	// disable scroll limits
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

void * titres_state_context;

void START(void) {
	// relocate SAT and name tables
	MAP_OVERLAP_SPR;
	// allocate coroutine context
	coro_runner_process(titres_state_context = coro_runner_alloc(TitresLogic, BANK(StateTitres), NULL));
}

void UPDATE(void) {
	// iterate coroutine
	coro_runner_process(titres_state_context);
}

void DESTROY(void) {
	// deallocate coroutine context
	coro_runner_free(titres_state_context);
	// relocate SAT and name tables
	MAP_OVERLAP_BKG;
}
