#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_ZERO 0
static const UINT8 anim_level[] = VECTOR(  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 );

extern UINT8 current_level;

#define SECONDS(A) ((A) * 60)

inline void UpdatePosition(void) {
#ifdef MASTERSYSTEM
	THIS->y = scroll_y + 8;
	THIS->x = scroll_x + (DEVICE_SCREEN_PX_WIDTH - 16 - 32 - 16);
#else
	THIS->y = scroll_y + 4;
	THIS->x = scroll_x + (DEVICE_SCREEN_PX_WIDTH - 16 - 8);
#endif
}

static void blink(UINT16 frames) {
	for (UINT16 timer = frames >> 3; (timer); --timer) {
		SetVisible(THIS, FALSE);
		for (UINT8 i = 4; (i); --i) {
			UpdatePosition();
			YIELD;
		}
		SetVisible(THIS, TRUE);
		for (UINT8 i = 4; (i); --i) {
			UpdatePosition();
			YIELD;
		}
	}
}

void LevelLogic(void * custom_data) BANKED {
	(void)custom_data;
	// set animation which displays the current level
	SetSpriteAnim(THIS, anim_level, ANIMATION_SPEED_ZERO);
	SetSpriteAnimFrame(THIS, current_level);
	// blink for 2 seconds
	blink(SECONDS(2));
	// stay on screen for 10 seconds
	for (UINT16 timer = SECONDS(10); (timer); --timer) {
		UpdatePosition();
		YIELD;
	}
	// blink for 2 seconds
	blink(SECONDS(2));
	// destroy itself
	return;
}

SPRITE_COROUTINE(LevelLogic, NONE)
