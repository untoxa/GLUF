// Slug enemy sprite walks left or right randomly, twice slower than the player; don't fall or use elevators

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_SLOW 10
static const UINT8 anim_slug_move_vert[]  = VECTOR( 0, 1 );
static const UINT8 anim_slug_move_horiz[] = VECTOR( 0, 2, 3, 1 );

static const UINT8 * const anim_slug[N_DIRECTIONS] = { anim_slug_move_vert, anim_slug_move_vert, anim_slug_move_vert, anim_slug_move_horiz, anim_slug_move_horiz };

static const INT8 x_delta[N_DIRECTIONS] = {  0,  0,  0, -MOVE_SPEED,  MOVE_SPEED };

void SlugLogic(void * custom_data) BANKED {
	enemy_dir_e old_direction = N_DIRECTIONS, direction = (chance_50_percent()) ? DIR_LEFT : DIR_RIGHT;
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_slug[direction], ANIMATION_SPEED_SLOW);
	for (;;) {
		old_direction = direction;
		if (direction) {
			switch (direction) {
				case DIR_LEFT:
					if ((x == 0) || (!check_collision(level_buffer[y + 1][x - 1]))) direction = DIR_RIGHT;
					break;
				case DIR_RIGHT:
					if ((x == (LEVEL_WIDTH) - 1) || (!check_collision(level_buffer[y + 1][x + 1]))) direction = DIR_LEFT;
					break;
				default:
					direction = DIR_NONE;
					break;

			}
			if (direction != old_direction) {
				SetSpriteAnim(THIS, anim_slug[direction], ANIMATION_SPEED_SLOW);
			}
			for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
				THIS->x += x_delta[direction];
				CheckKillGLUF(THIS);
				YIELD;
				CheckKillGLUF(THIS);
				YIELD;
			}
			x += x_delta[direction];
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
			CheckKillGLUF(THIS);
			YIELD;
		}
	}
}

SPRITE_COROUTINE(SlugLogic, NONE)
