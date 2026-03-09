// sparkle on the GLUF's monocle

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_IDLE 12
static const UINT8 anim_spark[] = VECTOR( 0, 1, 2, 3 );

void SparkLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetAnimationLoop(THIS, FALSE);
	SetSpriteAnim(THIS, anim_spark, ANIMATION_SPEED_IDLE);
	SetSpriteAnimFrame(THIS, 3);
	for (;;) {
		DELAY(((rand() % 10) + 2) * 60);
		SetSpriteAnimFrame(THIS, 0);
	}
}

SPRITE_COROUTINE(SparkLogic, NONE)
