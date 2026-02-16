#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Sprite.h"
#include "Sound.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "levels.h"

DECLARE_SFX(sfx10dead_nonoise);

#define ANIMATION_SPEED_IDLE 12
static const UINT8 anim_jumper_idle[] = VECTOR( 0, 1 );
#define ANIMATION_SPEED_JUMP  40
static const UINT8 anim_jumper_jump_right[] = VECTOR( 1, 2, 3, 4, 5, 6, 7, 1 );
static const UINT8 anim_jumper_jump_left[]  = VECTOR( 1, 7, 6, 5, 4, 3, 2, 1 );

extern Sprite * GLUF;
extern UINT8 restart;

static const INT8 x_delta[N_DIRECTIONS] = {  0,  0,  0, -1,  1 };
static const INT8 y_delta[N_DIRECTIONS] = {  0, -1,  1,  0,  0 };

void JumperLogic(void * custom_data) BANKED {
	enemy_dir_e old_direction = N_DIRECTIONS, direction = (rand() & 0x01u) ? DIR_LEFT : DIR_RIGHT;
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
	while (TRUE) {
		YIELD;
	}
}

void START(void) {
	INIT_CORO(BANK(SpriteJumper), JumperLogic);
}

void UPDATE(void) {
	ITER_CORO;
}

void DESTROY(void) {
	FREE_CORO;
}
