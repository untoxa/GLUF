// Ghost sprite, work similar to the UFO sprite but may randomly teleport itself from and to any of the specified locations

#include "Banks/SetAutoBank.h"

#include <rand.h>

#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

#define ANIMATION_SPEED_MOVE 10
static const UINT8 anim_ghost_move_vert[]  = VECTOR( 0, 1 );
static const UINT8 anim_ghost_move_horiz[] = VECTOR( 5, 1, 2, 3, 2, 4 );

#define ANIMATION_SPEED_TELEPORT 10
static const UINT8 anim_ghost_teleport[]   = VECTOR( 0, 6 );

static const UINT8 * const anim_ghost[N_DIRECTIONS] = { anim_ghost_move_vert, anim_ghost_move_vert, anim_ghost_move_vert, anim_ghost_move_horiz, anim_ghost_move_horiz };

static const INT8 x_delta[N_DIRECTIONS] = {  0,           0,           0, -MOVE_SPEED,  MOVE_SPEED };
static const INT8 y_delta[N_DIRECTIONS] = {  0, -MOVE_SPEED,  MOVE_SPEED,           0,           0 };

extern UINT8 teleport_count;
extern UINT8 teleport_x[];
extern UINT8 teleport_y[];

static const enemy_dir_e move_any_not_up[]    = {         DIR_DOWN, DIR_LEFT, DIR_RIGHT };
static const enemy_dir_e move_any_not_down[]  = { DIR_UP,           DIR_LEFT, DIR_RIGHT };
static const enemy_dir_e move_any_not_left[]  = { DIR_UP, DIR_DOWN,           DIR_RIGHT };
static const enemy_dir_e move_any_not_right[] = { DIR_UP, DIR_DOWN, DIR_LEFT            };

void GhostLogic(void * custom_data) BANKED {
	enemy_dir_e old_direction = N_DIRECTIONS, direction;
	switch (((UINT8 *)custom_data)[0]) {
		case ENEMY_GHOST_LEFT:
			direction = DIR_LEFT;
			break;
		case ENEMY_GHOST_RIGHT:
			direction = DIR_RIGHT;
			break;
		case ENEMY_GHOST_UP:
			direction = DIR_UP;
			break;
		case ENEMY_GHOST_DOWN:
			direction = DIR_DOWN;
			break;
		default:
			direction = DIR_NONE;
			break;
	};
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	SetSpriteAnim(THIS, anim_ghost[direction], ANIMATION_SPEED_MOVE);
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
			case MOVE_GHOST_POINT:
				if (chance_50_percent()) {
					// set teleport animation
					SetSpriteAnim(THIS, anim_ghost_teleport, ANIMATION_SPEED_TELEPORT);
					// disappear in the old teleport point
					for (UINT8 i = 0; i != SECONDS(1); ++i) {
						CheckKillGLUF(THIS);
						YIELD;
					}
					// choose random new teleport point
					UINT8 idx = rand() % teleport_count;
					// appear in the new teleport point
					THIS->x = ((x = teleport_x[idx]) << 4) + (TILE_BUFFER_OFFSET << 3);
					THIS->y = (y = teleport_y[idx]) << 4;
					for (UINT8 i = 0; i != SECONDS(1); ++i) {
						CheckKillGLUF(THIS);
						YIELD;
					}
					// set back move animation
					SetSpriteAnim(THIS, anim_ghost[direction], ANIMATION_SPEED_MOVE);
				}
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
				SetSpriteAnim(THIS, anim_ghost[direction], ANIMATION_SPEED_MOVE);
			}
			for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
				THIS->x += x_delta[direction];
				THIS->y += y_delta[direction];
				CheckKillGLUF(THIS);
				YIELD;
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

SPRITE_COROUTINE(GhostLogic, NONE)
