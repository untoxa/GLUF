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
#define ANIMATION_SPEED_FALL 10
static const UINT8 anim_jumper_fall[] = VECTOR( 0, 1 );

extern Sprite * GLUF;
extern UINT8 restart;

static const enemy_dir_e rand_directions[] = { DIR_NONE, DIR_UP, DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_UP };

void JumperLogic(void * custom_data) BANKED {
	enemy_dir_e direction = (rand() & 0x01u) ? DIR_LEFT : DIR_RIGHT;
	UINT8 tile_below;
	UINT8 x = ((UINT8 *)custom_data)[1];
	UINT8 y = ((UINT8 *)custom_data)[2];
	UINT8 falling = FALSE;
	UINT8 lifting = TILE_LIFT_NONE;
	SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
	while (TRUE) {
		if ((!falling) && (!lifting)) {
			// GLUF movements with joypad
			tile_below = level_buffer[y + 1][x];
			switch(direction) {
				case DIR_NONE:
					for (UINT8 i = 0; i != 16 ; ++i) {
						YIELD;
					}
					direction = rand_directions[rand() % sizeof(rand_directions)];
					break;
				case DIR_UP:
					if (check_lift(level_buffer[y - 1][x])) {
						lifting = TILE_LIFT_UP;
					} else {
//						direction = (rand() & 0x01) ? DIR_LEFT : DIR_RIGHT;
					}
					direction = (rand() & 0x01) ? DIR_LEFT : DIR_RIGHT;
					break;
				case DIR_DOWN:
					if (((check_lift(level_buffer[y][x]))) && (!check_collision(tile_below))) {
						lifting = TILE_LIFT_DOWN;
					} else {
						if (check_lift(tile_below) == TILE_LIFT_DOWN) {
							lifting = TILE_LIFT_DOWN;
						} else {
//							direction = (rand() & 0x01) ? DIR_LEFT : DIR_RIGHT;
						}
						direction = (rand() & 0x01) ? DIR_LEFT : DIR_RIGHT;
					}
					break;
				case DIR_LEFT:
					if (x > 0) {
						if (check_collision(level_buffer[y][x - 1]) == 0) {
							SetSpriteAnim(THIS, anim_jumper_jump_left, ANIMATION_SPEED_JUMP);
							for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
								THIS->x -= MOVE_SPEED;
								if ((GLUF) && (CheckCollision(THIS, GLUF))) {
									ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
									SpriteManagerRemoveSprite(GLUF);
									scroll_target = NULL;
									restart = TRUE;
								}
								YIELD;
							}
							SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
							x--;
							if (rand() & 0x03) direction = rand_directions[rand() % sizeof(rand_directions)];
						} else direction = DIR_NONE;
					} else direction = DIR_NONE;
					break;
				case DIR_RIGHT:
					if (x < (LEVEL_WIDTH - 1)) {
						if (check_collision(level_buffer[y][x + 1]) == 0) {
							SetSpriteAnim(THIS, anim_jumper_jump_right, ANIMATION_SPEED_JUMP);
							for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
								THIS->x += MOVE_SPEED;
								if ((GLUF) && (CheckCollision(THIS, GLUF))) {
									ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
									SpriteManagerRemoveSprite(GLUF);
									scroll_target = NULL;
									restart = TRUE;
								}
								YIELD;
							}
							SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
							x++;
							if (rand() & 0x03) direction = rand_directions[rand() % sizeof(rand_directions)];
						} else direction = DIR_NONE;
					} else direction = DIR_NONE;
					break;
				default:
					direction = DIR_NONE;
					break;
			}
		}
		// lifts and physics
		tile_below = level_buffer[y + 1][x];
		if (lifting == TILE_LIFT_UP) {
			SetSpriteAnim(THIS, anim_jumper_fall, ANIMATION_SPEED_FALL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y -= LIFT_SPEED;
				if ((GLUF) && (CheckCollision(THIS, GLUF))) {
					ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
					SpriteManagerRemoveSprite(GLUF);
					scroll_target = NULL;
					restart = TRUE;
				}
				YIELD;
			}
			y--;
			switch (check_lift(level_buffer[y][x])) {
				case TILE_LIFT_NONE:
				case TILE_LIFT_STOP:
					lifting = TILE_LIFT_NONE;
					SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
					break;
				default:
					break;
			}
		} else if (lifting == TILE_LIFT_DOWN) {
			SetSpriteAnim(THIS, anim_jumper_fall, ANIMATION_SPEED_FALL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y += LIFT_SPEED;
				if ((GLUF) && (CheckCollision(THIS, GLUF))) {
					ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
					SpriteManagerRemoveSprite(GLUF);
					scroll_target = NULL;
					restart = TRUE;
				}
				YIELD;
			}
			if ((!check_lift(tile_below)) || (check_collision(level_buffer[y + 2][x]))) {
				lifting = TILE_LIFT_NONE;
				SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
			}
			y++;
		} else {
			if (y < LEVEL_HEIGHT) {
				if (check_collision(tile_below) == 0) {
					falling = TRUE;
					y++;
					SetSpriteAnim(THIS, anim_jumper_fall, ANIMATION_SPEED_FALL);
					for (UINT8 i = 0; i != (16 / FALL_SPEED); ++i) {
						THIS->y += FALL_SPEED;
						if ((GLUF) && (CheckCollision(THIS, GLUF))) {
							ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
							SpriteManagerRemoveSprite(GLUF);
							scroll_target = NULL;
							restart = TRUE;
						}
						YIELD;
					}
				} else {
					falling = FALSE;
					SetSpriteAnim(THIS, anim_jumper_idle, ANIMATION_SPEED_IDLE);
				}
			} else {
				// kill itself, must never happen
				return;
			}
		}
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
