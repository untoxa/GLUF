#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_NONE 0
static const UINT8 anim_G[] = VECTOR( 0 );
static const UINT8 anim_L[] = VECTOR( 1 );
static const UINT8 anim_U[] = VECTOR( 2 );
static const UINT8 anim_F[] = VECTOR( 3 );

static const UINT8 * const anim_GLUF[] = { anim_G, anim_L, anim_U, anim_F };

static const INT8 sine_table[] = {
	0,2,4,6,7,9,11,12,13,14,15,15,16,15,15,14,13,12,11,9,7,6,4,2,0,-2,-4,-6,-8,-9,-11,-12,-13,-14,-15,-15,-16,-15,-15,-14,-13,-12,-11,-9,-8,-6,-4,-2
};

void SignLogic(void * custom_data) BANKED {
	INT16 y = THIS->y;
	UINT8 id = *((UINT8 *)custom_data);
	SetSpriteAnim(THIS, anim_GLUF[id], ANIMATION_SPEED_NONE);
	UINT8 counter = id * (sizeof(sine_table) / 8);
	while (TRUE) {
		THIS->y = y + sine_table[counter];
		if (++counter == sizeof(sine_table)) counter = 0;
		YIELD;
	}
}

SPRITE_COROUTINE(SignLogic, NONE)
