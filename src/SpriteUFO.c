#include "Banks/SetAutoBank.h"

#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "levels.h"

#define ANIMATION_SPEED_MOVE 12
static const UINT8 anim_ufo_move[] = VECTOR( 0, 1 );

extern Sprite * GLUF;
extern UINT8 restart;

static const INT8 x_delta[N_DIRECTIONS] = {  0,  0,  0, -1,  1 };
static const INT8 y_delta[N_DIRECTIONS] = {  0, -1,  1,  0,  0 };

void UFOLogic(void * custom_data) BANKED {
	enemy_dir_e direction;
	switch (((UINT8 *)custom_data)[0]) {
		case ENEMY_UFO_DOWN:
			direction = DIR_DOWN;
			break;
		case ENEMY_UFO_UP:
			direction = DIR_UP;
			break;
		default:
			direction = DIR_NONE;
			break;

	};
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_ufo_move, ANIMATION_SPEED_MOVE);
	while (TRUE) {
		if (CheckCollision(THIS, GLUF)) {
			restart = TRUE;
			YIELD;
		} else {
			switch (level_buffer[y][x]) {
				case MOVE_UP:
					direction = DIR_UP;
					break;
				case MOVE_DOWN:
					direction = DIR_DOWN;
					break;
			}
			if (direction) {
				for (UINT8 i = 0; i != 16; ++i) {
					THIS->x += x_delta[direction];
					THIS->y += y_delta[direction];
					YIELD;
				}
				x += x_delta[direction];
				y += y_delta[direction];
			} else {
				YIELD;
			}
		}
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
