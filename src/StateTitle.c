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

extern UINT8 current_level;

typedef struct {
	SPRITE_TYPE type;
	UINT8 x, y;     // in pixels
} sprite_coords_t;

const sprite_coords_t title_sprites[] = {
#if defined(MASTERSYSTEM)
	{SpriteTeslafrog, 152, 24}, {SpriteRetrosouls, 183, 102}, {SpriteTonyandco, 175, 70}, {SpriteSpark, 134, 56}
#elif defined(GAMEGEAR)
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 120,  88}, {SpriteTonyandco, 114, 56}, {SpriteSpark,  86, 42}
#else
	{SpriteTeslafrog,  92,  8}, {SpriteRetrosouls, 121,  89}, {SpriteTonyandco, 123, 54}, {SpriteSpark, 86, 42}
#endif
};

#ifdef ENABLE_CHEATS
extern UINT8 is_cheating;
static const UINT8 konami_code[] = { J_UP, J_UP, J_DOWN, J_DOWN, J_LEFT, J_RIGHT, J_LEFT, J_RIGHT, J_B, J_A };
static const UINT8 * konami_code_ptr = konami_code;

UINT8 ProcessCheatcodeSequence(void) {
	// process KNOAMI code, enable cheat mode if entered
	if (KEY_TICKED(*konami_code_ptr)) ++konami_code_ptr;
	else if (KEY_TICKED(~(*konami_code_ptr))) konami_code_ptr = konami_code;
	// code entered successfully?
	if (konami_code_ptr == (konami_code + sizeof(konami_code))) {
		// toggle the cheat mode on code enter
		is_cheating = !is_cheating;
		// play SFX
		ExecuteSFX(BANK(sfx11exitsound), sfx11exitsound, SFX_MUTE_MASK(sfx11exitsound), SFX_PRIORITY_NORMAL);
		// reset code sequence
		konami_code_ptr = konami_code;
		return TRUE;
	}
	return FALSE;
}
#endif

#if defined(ENABLE_CODES)
typedef struct {
	Sprite * sprite;
	UINT8 value;
} digit_t;

digit_t digits[4];

typedef struct {
	UINT8 param;    // letter
	UINT8 x, y;     // in tiles
	Sprite ** handle;
} code_coords_t;

Sprite * letter_O, * letter_K;

const code_coords_t code_sprites[] = {
	// code
	{10,  0,  0, NULL}, {11,  2,  0, NULL}, {12,  4,  0, NULL}, {13,  6,  0, NULL},
	// 0000
	{ 0,  0,  3, &digits[0].sprite}, { 0,  2,  3, &digits[1].sprite}, { 0,  4,  3, &digits[2].sprite}, { 0,  6,  3, &digits[3].sprite},
	// OK
	{11,  4,  6, &letter_O}, {14,  6,  6, &letter_K}
};

void CodeSetDigit(Sprite *sprite, UINT8 value) BANKED;
void CodeActivateDigit(Sprite *sprite, UINT8 value) BANKED;

UINT8 get_level_by_code(UINT16 code) BANKED;

UINT8 EnterCode(void) {
	digit_t * active_digit = NULL, * old_active_digit = digits;

	SpriteManagerReset();
	YIELD;

	for (UINT8 i = 0; i != ARRAY_LENGTH(code_sprites); ++i) {
		UINT8 tx = ((SCREEN_TILES_W - 8) / 2) + code_sprites[i].x;
		UINT8 ty = ((SCREEN_TILES_H - 8) / 2) + code_sprites[i].y;
		Sprite * sprite = SpriteManagerAdd(SpriteCode, tx << 3, ty << 3);
		if (code_sprites[i].handle) *(code_sprites[i].handle) = sprite;
		sprite->custom_data[0] = SpriteCode;
		sprite->custom_data[1] = tx;
		sprite->custom_data[2] = ty;
		sprite->custom_data[3] = code_sprites[i].param;
	}

	for(;; YIELD) {
#if defined(ENABLE_CHEATS)
		if (ProcessCheatcodeSequence()) continue;
#endif
		if (active_digit) {
			if (KEY_TICKED(J_UP)) {
				if (++(active_digit->value) == 10) active_digit->value = 0;
				CodeSetDigit(active_digit->sprite, active_digit->value);
			} else if (KEY_TICKED(J_DOWN)) {
				active_digit->value = (active_digit->value) ? (active_digit->value - 1) : 9;
				CodeSetDigit(active_digit->sprite, active_digit->value);
			} else if (KEY_TICKED(J_RIGHT)) {
				if (++active_digit > (digits + 3)) active_digit = NULL;
			} else if (KEY_TICKED(J_LEFT)) {
				if (--active_digit < digits) active_digit = NULL;
			}

		} else {
			if ((KEY_TICKED(J_RIGHT)) || (KEY_TICKED(J_UP))) {
				active_digit = digits;
			} else if ((KEY_TICKED(J_LEFT)) || (KEY_TICKED(J_DOWN))) {
				active_digit = digits + 3;
			} else if (KEY_TICKED(J_A)) {
				UINT16 code = ((digits[0].value & 0x0f) << 12) | ((digits[1].value & 0x0f) << 8) | ((digits[2].value & 0x0f) << 4) | (digits[3].value & 0x0f);
				return get_level_by_code(code);
			}
		}
		if (active_digit != old_active_digit) {
			if (old_active_digit) {
				CodeActivateDigit(old_active_digit->sprite, FALSE);
			} else {
				CodeActivateDigit(letter_O, FALSE);
				CodeActivateDigit(letter_K, FALSE);
			}
			if (active_digit) {
				CodeActivateDigit(active_digit->sprite, TRUE);
			} else {
				CodeActivateDigit(letter_O, TRUE);
				CodeActivateDigit(letter_K, TRUE);
			}
			old_active_digit = active_digit;
		}
	}
}
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
#if defined(ENABLE_CODES)
		if (KEY_TICKED(J_B)) {
			current_level = EnterCode();
			// init RNG
			initarand(((UINT16)DIV_REG << 8) ^ sys_time);
			// start game
			SetState(StateGame);
		} else
#else
	#if defined(ENABLE_CHEATS)
		if (ProcessCheatcodeSequence()) continue;
	#endif
#endif
		if (KEY_TICKED(START_BUTTONS)) {
			// start from the initial level
			current_level = INITIAL_LEVEL_NUMBER;
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
