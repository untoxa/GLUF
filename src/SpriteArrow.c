// arrow sprite hints player what to do on the tutorial levels

#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "Sprite.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_IDLE 0
static const UINT8 anim_arrow[] = VECTOR( 0, 1 );

const INT8 sine[] = { 0,  0,  0,  1,  1,  2,  2,  2,  2,  2,  2,  2,  1,  1,  0,  0,  0,  0,  0, -1, -1, -2, -2, -2, -2, -2, -2, -2, -1, -1,  0,  0 };

extern Sprite * GLUF;

void ArrowLogic(void * custom_data) BANKED {
	(void)custom_data;
	INT16 y = THIS->y;
	const INT8 * ptr = sine + ((THIS->custom_data[0] == TILE_ARROW_DOWN) ? 0 : (sizeof(sine) >> 1));
	SetSpriteAnim(THIS, anim_arrow, ANIMATION_SPEED_IDLE);
	SetSpriteAnimFrame(THIS, (THIS->custom_data[0] == TILE_ARROW_DOWN) ? 0 : 1);
	for (;; YIELD) {
		THIS->y = y + *ptr;
		if (++ptr == sine + sizeof(sine)) ptr = sine;
		if (!GLUF) return;
	}
}

SPRITE_COROUTINE(ArrowLogic, NONE)
