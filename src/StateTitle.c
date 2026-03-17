// title screen state - shows the title screen, put sprite logos over it, then wait for A/START;
// also checks for entering the konami code which enables fast switching between the levels

#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "Music.h"
#include "Coroutines.h"
#include "SGB.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

IMPORT_BORDER(border);

IMPORT_MAP(title);
IMPORT_MAP(lockscreen);

DECLARE_SFX(sfx11exitsound);

typedef struct {
	SPRITE_TYPE type;
	UINT8 x, y;
} sprite_coords_t;

const sprite_coords_t title_sprites[] = {
#if defined(MASTERSYSTEM)
	{SpriteTeslafrog, 152, 24}, {SpriteRetrosouls, 183, 102}, {SpriteTonyandco, 175, 70}, {SpriteSpark, 134, 56}
#elif defined(GAMEGEAR)
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 120,  88}, {SpriteTonyandco, 114, 56}, {SpriteSpark, 86, 42}
#else
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 121,  89}, {SpriteTonyandco, 123, 54}, {SpriteSpark, 86, 42}
#endif
};

#ifdef ENABLE_CHEATS
extern UINT8 is_cheating;
static const UINT8 konami_code[] = { J_UP, J_UP, J_DOWN, J_DOWN, J_LEFT, J_RIGHT, J_LEFT, J_RIGHT, J_B, J_A };
static const UINT8 * konami_code_ptr = konami_code;
#endif

NORETURN void TitleLogic(void * custom_data) BANKED {
	(void)custom_data;
	// load SGB border
	LOAD_SGB_BORDER(border);
	// set up CrossZGB scrolling parameters
	SPRITES_8x16;
	MAP_OVERLAP_SPR;
	DISABLE_SCROLL_CLAMPING;
	// set up and start music
	stop_music_on_new_state = FALSE;
	music_enable_NTSC_compensation();
	load_music(MUSIC_INTRO);
	// destroy all sprites
	SpriteManagerReset();
#if defined(NINTENDO)
	// lock screen if not support color
	if (!DEVICE_SUPPORTS_COLOR) {
		// init background with the lock screen
		InitScroll(BANK(lockscreen), &lockscreen, NULL, NULL);
		// wait forever
		for(;; YIELD);
	}
#endif
	// some parts of the screen are sprites
	for (UINT8 i = 0; i != ARRAY_LENGTH(title_sprites); ++i) {
		SpriteManagerAdd(title_sprites[i].type, title_sprites[i].x, title_sprites[i].y);
	}
	// initialize background
	InitScroll(BANK(title), &title, NULL, NULL);
	// compensate hidden column on SMS so background is centered
	CompensateScroll();

	// the first YIELD call separate initialization of the state from processing
	// for example, unfading happens there
	YIELD;

	for (;; YIELD) {
#ifdef ENABLE_CHEATS
		// process KNOAMI code, enable cheat mode if entered
		if (KEY_TICKED(*konami_code_ptr)) ++konami_code_ptr;
		else if (KEY_TICKED(~(*konami_code_ptr))) konami_code_ptr = konami_code;
		if (konami_code_ptr == (konami_code + sizeof(konami_code))) {
			// toggle the cheat mode on code enter
			is_cheating = !is_cheating;
			// play SFX
			ExecuteSFX(BANK(sfx11exitsound), sfx11exitsound, SFX_MUTE_MASK(sfx11exitsound), SFX_PRIORITY_NORMAL);
			// reset code sequence
			konami_code_ptr = konami_code;
		} else
#endif
		if (KEY_TICKED(START_BUTTONS)) {
			// init RNG
			initarand(((UINT16)DIV_REG << 8) ^ sys_time);
			// start game
			SetState(StateGame);
		}
	}
}

void TitleLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	MAP_OVERLAP_BKG;
	SPRITES_8x8;
}

STATE_COROUTINE(TitleLogic, TitleLogicFinalizer)
