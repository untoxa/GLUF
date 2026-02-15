#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "levels.h"

#define ANIMATION_SPEED_SLOW 10
static const UINT8 anim_UFO_move_vert[]  = VECTOR( 0, 1 );
static const UINT8 anim_UFO_move_horiz[] = VECTOR( 0, 2, 3, 1 );

static const UINT8 * const anim_UFO[N_DIRECTIONS] = { anim_UFO_move_vert, anim_UFO_move_vert, anim_UFO_move_vert, anim_UFO_move_horiz, anim_UFO_move_horiz };

extern Sprite * GLUF;
extern UINT8 restart;

static const INT8 x_delta[N_DIRECTIONS] = {  0,  0,  0, -1,  1 };
static const INT8 y_delta[N_DIRECTIONS] = {  0, -1,  1,  0,  0 };

void SlugLogic(void * custom_data) BANKED {
	enemy_dir_e old_direction = N_DIRECTIONS, direction = (rand() & 1) ? DIR_LEFT : DIR_RIGHT;
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_UFO[direction], ANIMATION_SPEED_SLOW);
	while (TRUE) {
		old_direction = direction;
		if (direction) {
			switch (direction) {
				case DIR_LEFT:
					if ((x == 0) || ((level_buffer[y+1][x-1] & TILE_VISIBLE_MASK) == TILE_EMPTY)) direction = DIR_RIGHT;
					break;
				case DIR_RIGHT:
					if ((x == (LEVEL_WIDTH) - 1) || ((level_buffer[y+1][x+1] & TILE_VISIBLE_MASK) == TILE_EMPTY)) direction = DIR_LEFT;
					break;
				default:
					direction = DIR_NONE;
					break;

			}
			if (direction != old_direction) {
				SetSpriteAnim(THIS, anim_UFO[direction], ANIMATION_SPEED_SLOW);
			}
			for (UINT8 i = 0; i != 16; ++i) {
				THIS->x += x_delta[direction];
				THIS->y += y_delta[direction];
				if (CheckCollision(THIS, GLUF)) restart = TRUE;
				YIELD;
				if (CheckCollision(THIS, GLUF)) restart = TRUE;
				YIELD;
			}
			x += x_delta[direction];
			y += y_delta[direction];
			// random change direction
			switch (rand() & 0x07) {
				case 0:
					direction = DIR_LEFT;
					break;
				case 1:
					direction = DIR_RIGHT;
					break;
			}
		} else {
			if (CheckCollision(THIS, GLUF)) restart = TRUE;
			YIELD;
		}
	}
}

void START(void) {
	INIT_CORO(BANK(SpriteSlug), SlugLogic);
}

void UPDATE(void) {
	ITER_CORO;
}

void DESTROY(void) {
	FREE_CORO;
}
