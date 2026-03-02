#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_battery[] = VECTOR( 0, 1 );

void BatteryLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetSpriteAnim(THIS, anim_battery, ANIMATION_SPEED_IDLE);
	if (THIS->custom_data[0] == TILE_BATT_CHARGED) SetSpriteAnimFrame(THIS, 1);
	THIS->y++;
	YIELD;
	THIS->y++;
	YIELD;
	YIELD;
	THIS->y--;
	YIELD;
	THIS->y--;
	YIELD;
}

SPRITE_COROUTINE(BatteryLogic, NONE)
