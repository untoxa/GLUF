// code sprites are used for entering the level code and for displaying those codes after the each level

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_code[]  = VECTOR( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 );

void CodeSetDigit(Sprite *sprite, UINT8 value) BANKED {
	if (!sprite) return;
	if (value < VECTOR_LEN(anim_code)) SetSpriteAnimFrame(sprite, value);
}
void CodeActivateDigit(Sprite *sprite, UINT8 value) BANKED {
	if (!sprite) return;
	sprite->custom_data[4] = value;
}

void CodeLogic(void * custom_data) BANKED {
	INT16 tx = ((UINT8 *)custom_data)[1];
	INT16 ty = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_code, ANIMATION_SPEED_IDLE);
	SetSpriteAnimFrame(THIS, ((UINT8 *)custom_data)[3]);
	UINT16 x = THIS->x, y = THIS->y;
	for (;; YIELD) {
		if (sys_time & 0x01) continue;
		if (THIS->custom_data[4]) {
			THIS->x = (x - 2) + (rand() & 0x03);
			THIS->y = (y - 2) + (rand() & 0x03);
		} else {
			THIS->x = x;
			THIS->y = y;
		}
	}
}

SPRITE_COROUTINE(CodeLogic, NONE)
