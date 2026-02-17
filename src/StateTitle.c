#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Coroutines.h"

IMPORT_MAP(title);

void TitleLogic(void * custom_data) BANKED {
	(void)custom_data;
	// disable scroll limits
	clamp_enabled = FALSE;
	// destroy all sprites
	SpriteManagerReset();
	// some parts of the screen are sprites
#ifdef MASTERSYSTEM
	SpriteManagerAdd(SpriteTeslafrog, 152, 24);
	SpriteManagerAdd(SpriteRetrosouls, 187, 104);
#else
	SpriteManagerAdd(SpriteTeslafrog, 92, 8);
	SpriteManagerAdd(SpriteRetrosouls, 120, 88);
#endif
	// initialize background with collisions (skip the very first tile (19), which is only for the player)
	InitScroll(BANK(title), &title, NULL, NULL);
#ifdef MASTERSYSTEM
	MoveScroll(8, 0);
#endif
	YIELD;
	while (TRUE) {
		if ((KEY_TICKED(J_A)) || KEY_TICKED(J_START)) {
			SetState(StateGame);
			return;
		}
		YIELD;
	}
}

void * title_state_context;

void START(void) {
	// relocate SAT and name tables
	MAP_OVERLAP_SPR;
	// allocate coroutine context
	coro_runner_process(title_state_context = coro_runner_alloc(TitleLogic, BANK(StateTitle), NULL));
}

void UPDATE(void) {
	// iterate coroutine
	coro_runner_process(title_state_context);
}

void DESTROY(void) {
	// deallocate coroutine context
	coro_runner_free(title_state_context);
	// relocate SAT and name tables
	MAP_OVERLAP_BKG;
}
