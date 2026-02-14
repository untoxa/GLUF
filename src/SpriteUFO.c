#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

#define ANIMATION_SPEED_MOVE 12
static const UINT8 anim_ufo_move[] = VECTOR( 0, 1 );

void UFOLogic(void * custom_data) BANKED {
	(void)custom_data;
	SetSpriteAnim(THIS, anim_ufo_move, ANIMATION_SPEED_MOVE);
	while (TRUE) {
		YIELD;
	}
}

void START(void) {
	INIT_CORO(BANK(SpriteUFO), UFOLogic);
}

void UPDATE(void) {
	ITER_CORO;
}

void DESTROY(void) {
	FREE_CORO;
}
