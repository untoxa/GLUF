// bolt of lightning, spawned when GLUF is charging; fires itself at random height with random speed of 1 or 2

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_bolt[] = VECTOR( 0 );

void BoltLogic(void * custom_data) BANKED {
	(void)custom_data;
	INT16 delta = (chance_50_percent()) ? -1 : -2;
	THIS->x += (rand() % 12) + 2;
	SetSpriteAnim(THIS, anim_bolt, ANIMATION_SPEED_IDLE);
	for (UINT8 i = (rand() % 16) + 8; i != 0; --i) {
		THIS->y += delta;
		YIELD;
	};
}

SPRITE_COROUTINE(BoltLogic, NONE)
