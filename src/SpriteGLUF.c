#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "Keys.h"
#include "ZGBMain.h"

#include "levels.h"

extern UINT8 restart;

UINT8 falling;
UINT8 lifting;
UINT8 player_x, player_y;
Sprite * GLUF;

#define MOVE_SPEED 1
#define FALL_SPEED 2
#define LIFT_SPEED 2

#define TILE_LIFT_NONE 0
#define TILE_LIFT_UP   5
#define TILE_LIFT_DOWN 10

#define ANIMATION_SPEED_IDLE 12
static const UINT8 anim_idle[]       = VECTOR( 0,  1 );
#define ANIMATION_SPEED_JUMP 40
static const UINT8 anim_jump_right[] = VECTOR( 2,  3,  4,  5,  6,  7,  8,  2 );
static const UINT8 anim_jump_left[]  = VECTOR( 2,  8,  7,  6,  5,  4,  3,  2 );
#define ANIMATION_SPEED_FALL 10
static const UINT8 anim_fall[]       = VECTOR( 9,  10 );

UINT8 check_collision(UINT8 id) {
	if (id < 10) return 0; return id;
}
UINT8 check_lift(UINT8 id) {
	if ((id == TILE_LIFT_UP) || (id == TILE_LIFT_DOWN)) return id; else return TILE_LIFT_NONE;
}

void GLUFLogic(void * custom_data) BANKED {
	(void)custom_data;
	falling = FALSE;
	lifting = 0;
	SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
	while (TRUE) {
		if ((!falling) && (!lifting)) {
			// moving left and right
			if (KEY_PRESSED(J_RIGHT)) {
				if (player_x < (LEVEL_WIDTH - 1)) {
					if (check_collision(level_buffer[player_y][player_x + 1]) == 0) {
						SetSpriteAnim(THIS, anim_jump_right, ANIMATION_SPEED_JUMP);
						for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
							THIS->x += MOVE_SPEED;
							YIELD;
						}
						player_x++;
						SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
					}
				}
			} else if (KEY_PRESSED(J_LEFT)) {
				if (player_x > 0) {
					if (check_collision(level_buffer[player_y][player_x - 1]) == 0) {
						SetSpriteAnim(THIS, anim_jump_left, ANIMATION_SPEED_JUMP);
						for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
							THIS->x -= MOVE_SPEED;
							YIELD;
						}
						player_x--;
						SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
					}
				}
			}
			// move up and down using lift
			if (KEY_PRESSED(J_UP)) {
				if (check_lift(level_buffer[player_y][player_x])) lifting = TILE_LIFT_UP;
			} else if (KEY_PRESSED(J_DOWN)) {
				if (((check_lift(level_buffer[player_y][player_x]))) && (!check_collision(level_buffer[player_y + 1][player_x]))) lifting = TILE_LIFT_DOWN;
				else if (check_lift(level_buffer[player_y + 1][player_x]) == TILE_LIFT_DOWN) lifting = TILE_LIFT_DOWN;
			}
		}
		// physics
		if (lifting == TILE_LIFT_UP) {
			SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y -= LIFT_SPEED;
				YIELD;
			}
			player_y--;
			if (!check_lift(level_buffer[player_y][player_x])) {
				lifting = TILE_LIFT_NONE;
				SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
			}
		} else if (lifting == TILE_LIFT_DOWN) {
			SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y += LIFT_SPEED;
				YIELD;
			}
			player_y++;
			if ((!check_lift(level_buffer[player_y][player_x])) || (check_collision(level_buffer[player_y + 1][player_x]))) {
				lifting = TILE_LIFT_NONE;
				SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
			}
		} else {
			if (player_y < LEVEL_HEIGHT) {
				if (check_collision(level_buffer[player_y + 1][player_x]) == 0) {
					falling = TRUE;
					player_y++;
					SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
					for (UINT8 i = 0; i != (16 / FALL_SPEED); ++i) {
						THIS->y += FALL_SPEED;
						YIELD;
					}
				} else {
					falling = FALSE;
					SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
				}
			} else restart = TRUE;
		}
		YIELD;
	}
}

void START(void) {
	INIT_CORO(BANK(SpriteGLUF), GLUFLogic);
}

void UPDATE(void) {
	ITER_CORO;
}

void DESTROY(void) {
	FREE_CORO;
}
