#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_bolt[] = VECTOR( 0 );

static const INT8 sine[] = {  0,  3,  5,  7,  8,  7,  5,  3,  0, -3, -5, -7, -8, -7, -5, -3 };

void BoltLogic(void * custom_data) BANKED {
	(void)custom_data;
	UINT8 phase = rand() % sizeof(sine);
	INT16 x = (THIS->x + (sine[phase] >> 2)) << 2;
	SetSpriteAnim(THIS, anim_bolt, ANIMATION_SPEED_IDLE);
	for (UINT8 i = 16; i != 0; --i) {
		THIS->y--;
		THIS->x = (x + sine[phase]) >> 2;
		if (++phase == sizeof(sine)) phase = 0;
		YIELD;
		YIELD;
	};
}

SPRITE_COROUTINE(BoltLogic, NONE)
