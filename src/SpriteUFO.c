// UFO enemy sprite - goes between the control points, randomly changing direction at those points

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_MOVE 20
static const UINT8 anim_UFO_move_vert[]  = VECTOR( 0, 1 );
static const UINT8 anim_UFO_move_horiz[] = VECTOR( 0, 2, 3, 4 );

static const UINT8 * const anim_UFO[N_DIRECTIONS] = { anim_UFO_move_vert, anim_UFO_move_vert, anim_UFO_move_vert, anim_UFO_move_horiz, anim_UFO_move_horiz };

static const INT8 x_delta[N_DIRECTIONS] = {  0,           0,           0, -MOVE_SPEED,  MOVE_SPEED };
static const INT8 y_delta[N_DIRECTIONS] = {  0, -MOVE_SPEED,  MOVE_SPEED,           0,           0 };

static const enemy_dir_e move_any_not_up[]    = {         DIR_DOWN, DIR_LEFT, DIR_RIGHT };
static const enemy_dir_e move_any_not_down[]  = { DIR_UP,           DIR_LEFT, DIR_RIGHT };
static const enemy_dir_e move_any_not_left[]  = { DIR_UP, DIR_DOWN,           DIR_RIGHT };
static const enemy_dir_e move_any_not_right[] = { DIR_UP, DIR_DOWN, DIR_LEFT            };

void UFOLogic(void * custom_data) BANKED {
	enemy_dir_e old_direction = N_DIRECTIONS, direction;
	switch (((UINT8 *)custom_data)[0]) {
		case ENEMY_UFO_LEFT:
			direction = DIR_LEFT;
			break;
		case ENEMY_UFO_RIGHT:
			direction = DIR_RIGHT;
			break;
		case ENEMY_UFO_UP:
			direction = DIR_UP;
			break;
		case ENEMY_UFO_DOWN:
			direction = DIR_DOWN;
			break;
		default:
			direction = DIR_NONE;
			break;
	};
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_UFO[direction], ANIMATION_SPEED_MOVE);
	while (TRUE) {
		old_direction = direction;
		switch (level_buffer[y][x]) {
			case MOVE_LEFT:
				direction = DIR_LEFT;
				break;
			case MOVE_RIGHT:
				direction = DIR_RIGHT;
				break;
			case MOVE_UP:
				direction = DIR_UP;
				break;
			case MOVE_DOWN:
				direction = DIR_DOWN;
				break;
			case MOVE_RIGHT_OR_UP:
				direction = (chance_50_percent()) ? DIR_RIGHT : DIR_UP;
				break;
			case MOVE_LEFT_OR_UP:
				direction = (chance_50_percent()) ? DIR_LEFT : DIR_UP;
				break;
			case MOVE_RIGHT_OR_DOWN:
				direction = (chance_50_percent()) ? DIR_RIGHT : DIR_DOWN;
				break;
			case MOVE_LEFT_OR_DOWN:
				direction = (chance_50_percent()) ? DIR_LEFT : DIR_DOWN;
				break;
			case MOVE_ANY_NOT_DOWN:
				direction = ARRAY_PICK_RANDOM(move_any_not_down);
				break;
			case MOVE_ANY_NOT_UP:
				direction = ARRAY_PICK_RANDOM(move_any_not_up);
				break;
			case MOVE_ANY_NOT_RIGHT:
				direction = ARRAY_PICK_RANDOM(move_any_not_right);
				break;
			case MOVE_ANY_NOT_LEFT:
				direction = ARRAY_PICK_RANDOM(move_any_not_left);
				break;
			case MOVE_LEFT_OR_RIGHT:
				direction = (chance_50_percent()) ? DIR_LEFT : DIR_RIGHT;
				break;
			case MOVE_UP_OR_DOWN:
				direction = (chance_50_percent()) ? DIR_UP : DIR_DOWN;
				break;
			case MOVE_ANY:
				if (chance_50_percent()) direction = (rand() & 0x03) + DIR_UP;
				break;

		}
		if (direction) {
			// check out of bounds
			if ((x == 0) && (direction == DIR_LEFT)) direction = DIR_RIGHT;
			else if ((x == (LEVEL_WIDTH - 1)) && (direction == DIR_RIGHT)) direction = DIR_LEFT;
			if ((y == 0) && (direction == DIR_UP)) direction = DIR_DOWN;
			else if ((x == (LEVEL_HEIGHT - 1)) && (direction == DIR_DOWN)) direction = DIR_UP;
			// move into direction
			if (direction != old_direction) {
				SetSpriteAnim(THIS, anim_UFO[direction], ANIMATION_SPEED_MOVE);
			}
			for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
				THIS->x += x_delta[direction];
				THIS->y += y_delta[direction];
				CheckKillGLUF(THIS);
				YIELD;
			}
			x += x_delta[direction];
			y += y_delta[direction];
		} else {
			CheckKillGLUF(THIS);
			YIELD;
		}
	}
}

SPRITE_COROUTINE(UFOLogic, NONE)
