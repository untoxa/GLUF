// spark tail when using lift

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_IDLE 20
static const UINT8 anim_spark[] = VECTOR( 0, 1, 2, 3 );

void LiftSparkLogic(void * custom_data) BANKED {
	(void)custom_data;
	THIS->x += (rand() % 12) + 2;
	SetAnimationLoop(THIS, FALSE);
	SetSpriteAnim(THIS, anim_spark, ANIMATION_SPEED_IDLE);
	DELAY(20);
}

SPRITE_COROUTINE(LiftSparkLogic, NONE)
