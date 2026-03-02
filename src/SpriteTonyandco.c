// blinking "Tony&Co" logo sprite for the title screen

#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_FLICKER 3
static const UINT8 anim_tonyandco[] = VECTOR( 0, 1 );

void TonyAndCoLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetSpriteAnim(THIS, anim_tonyandco, ANIMATION_SPEED_FLICKER);
	for (;; YIELD);
}

SPRITE_COROUTINE(TonyAndCoLogic, NONE)
