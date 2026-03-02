// door sprite indicates the exit when it is activated after charging all batteries

#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_FLICKER 50
static const UINT8 anim_door[] = VECTOR( 0, 1 );

void DoorLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetSpriteAnim(THIS, anim_door, ANIMATION_SPEED_FLICKER);
	for (;; YIELD);
}

SPRITE_COROUTINE(DoorLogic, NONE)
