#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Scroll.h"
#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "levels.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_fire_idle[] = VECTOR( 0 );

static const INT8 sine_table[] = {
	0,1,3,4,5,6,7,7,8,7,7,6,5,4,3,1,0,-1,-3,-4,-5,-6,-7,-7,-8,-7,-7,-6,-5,-4,-3,-1
};

void FireLogic(void * custom_data) BANKED {
	(void)custom_data;
	UINT8 counter = 0;
	THIS->x = scroll_x + (((SCREEN_WIDTH - (SCREEN_BKG_OFFSET_X << 3)) >> 1));
	INT16 y = THIS->y = scroll_y + (SCREEN_HEIGHT >> 1);
	SetSpriteAnim(THIS, anim_fire_idle, ANIMATION_SPEED_IDLE);
	while (TRUE) {
		THIS->y = y + sine_table[counter];
		if (++counter == sizeof(sine_table)) counter = 0;
		YIELD;
	}
}

SPRITE_COROUTINE(FireLogic, NONE)
