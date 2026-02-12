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

#define TILE_LIFT_NONE 0
#define TILE_LIFT_UP   5
#define TILE_LIFT_DOWN 10

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
	while (TRUE) {
		// moving left and right
		if (!falling) {
			if (KEY_PRESSED(J_RIGHT)) {
				if (player_x < (LEVEL_WIDTH - 1)) {
					if (check_collision(level_buffer[player_y][player_x + 1]) == 0) {
						for (UINT8 i = 0; i != 16; ++i) {
							THIS->x++;
							YIELD;
						}
						player_x++;
						lifting = TILE_LIFT_NONE;
					}
				}
			} else if (KEY_PRESSED(J_LEFT)) {
				if (player_x > 0) {
					if (check_collision(level_buffer[player_y][player_x - 1]) == 0) {
						for (UINT8 i = 0; i != 16; ++i) {
							THIS->x--;
							YIELD;
						}
						player_x--;
						lifting = TILE_LIFT_NONE;
					}
				}
			}
		}
		// use lift
		if (KEY_PRESSED(J_UP)) {
			if (check_lift(level_buffer[player_y][player_x])) lifting = TILE_LIFT_UP, falling = FALSE;
		} else if (KEY_PRESSED(J_DOWN)) {
			if (((check_lift(level_buffer[player_y][player_x]))) && (!check_collision(level_buffer[player_y + 1][player_x]))) lifting = TILE_LIFT_DOWN, falling = FALSE;
			else if (check_lift(level_buffer[player_y + 1][player_x]) == TILE_LIFT_DOWN) lifting = TILE_LIFT_DOWN, falling = FALSE;
		}
		// physics
		if (lifting == TILE_LIFT_UP) {
			for (UINT8 i = 0; i != 16; ++i) {
				THIS->y--;
				YIELD;
			}
			player_y--;
			if (!check_lift(level_buffer[player_y][player_x])) lifting = TILE_LIFT_NONE;
		} else if (lifting == TILE_LIFT_DOWN) {
			for (UINT8 i = 0; i != 16; ++i) {
				THIS->y++;
				YIELD;
			}
			player_y++;
			if ((!check_lift(level_buffer[player_y][player_x])) || (check_collision(level_buffer[player_y + 1][player_x]))) lifting = TILE_LIFT_NONE;
		} else {
			if (player_y < LEVEL_HEIGHT) {
				if (check_collision(level_buffer[player_y + 1][player_x]) == 0) {
					falling = TRUE;
					player_y++;
					for (UINT8 i = 0; i != 16; ++i) {
						THIS->y++;
						YIELD;
					}
				} else falling = FALSE;
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
