#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "Music.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

IMPORT_MAP(title);

DECLARE_MUSIC(polkka);

typedef struct {
	SPRITE_TYPE type;
	UINT8 x, y;
} sprite_coords_t;

const sprite_coords_t title_sprites[] = {
#if defined(MASTERSYSTEM)
	{SpriteTeslafrog, 152, 24}, {SpriteRetrosouls, 183, 102}, {SpriteTonyandco, 175, 70}
#elif defined(GAMEGEAR)
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 120,  88}, {SpriteTonyandco, 114, 56}
#else
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 121,  89}, {SpriteTonyandco, 123, 54}
#endif
};

NORETURN void TitleLogic(void * custom_data) BANKED {
	(void)custom_data;
	// set up CrossZGB scrolling parameters
	SPRITES_8x16;
	MAP_OVERLAP_SPR;
	clamp_enabled = FALSE;
	// start music
	stop_music_on_new_state = FALSE;
	PlayMusic(polkka, 1);
	// destroy all sprites
	SpriteManagerReset();
	// some parts of the screen are sprites
	for (UINT8 i = 0; i != (sizeof(title_sprites)/sizeof(title_sprites[0])); ++i) {
		SpriteManagerAdd(title_sprites[i].type, title_sprites[i].x, title_sprites[i].y);
	}
	// initialize background
	InitScroll(BANK(title), &title, NULL, NULL);
	// compensate hidden column on SMS so background is centered
	CompensateScroll();
	YIELD;

	for (;; YIELD) {
		if ((KEY_TICKED(J_A)) || KEY_TICKED(J_START)) {
			SetState(StateGame);
			// wait until the CrossZGB engine switch the states
			for (;; YIELD);
		}
	}
}

void TitleLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	MAP_OVERLAP_BKG;
	SPRITES_8x8;
}

STATE_COROUTINE(TitleLogic, TitleLogicFinalizer)
