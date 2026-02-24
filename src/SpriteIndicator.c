#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_ZERO 0
static const UINT8 anim_charge[] = VECTOR(  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 );

extern UINT8 GLUF_charge;

Sprite * charge_indicator;

inline void UpdatePosition(void) {
#ifdef MASTERSYSTEM
	THIS->y = scroll_y + (DEVICE_SCREEN_PX_HEIGHT - 12);
	THIS->x = scroll_x + (DEVICE_SCREEN_PX_WIDTH - 40 - 32 - 16);
#else
	THIS->y = scroll_y + (DEVICE_SCREEN_PX_HEIGHT - 12);
	THIS->x = scroll_x + (DEVICE_SCREEN_PX_WIDTH - 40 - 8);
#endif
}

void IndicatorLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetSpriteAnim(THIS, anim_charge, ANIMATION_SPEED_ZERO);
	while (TRUE) {
		SetSpriteAnimFrame(THIS, GLUF_charge);
		UpdatePosition();
		YIELD;
	}
}

SPRITE_COROUTINE(IndicatorLogic, NONE)
