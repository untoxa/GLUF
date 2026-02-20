#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "Keys.h"
#include "Sound.h"
#include "ZGBMain.h"

#include "levels.h"

DECLARE_SFX(sfx1jump);
DECLARE_SFX(sfx3fall);
DECLARE_SFX(sfx4lift);
DECLARE_SFX(sfx5paint);
DECLARE_SFX(sfx6noname);
DECLARE_SFX(sfx7exit);
DECLARE_SFX(sfx10dead_nonoise);
DECLARE_SFX(sfx11exitsound);

extern UINT8 restart;
extern UINT8 battery_count;
extern UINT8 current_level;

extern Sprite * charge_indicator;

#define CHARGE_COOLDOWN 10
#define CHARGE_MAXIMUM  10

UINT8 start_x, start_y;
UINT8 GLUF_charge;
Sprite * GLUF;

#define ANIMATION_SPEED_IDLE  12
static const UINT8 anim_idle[]       = VECTOR(  0,  1 );
#define ANIMATION_SPEED_ENTER 15
static const UINT8 anim_enter[]      = VECTOR( 17, 16, 15, 14, 13, 12, 11 );
static const UINT8 anim_exit[]       = VECTOR( 11, 12, 13, 14, 15, 16, 17 );
#define ANIMATION_SPEED_JUMP  40
static const UINT8 anim_jump_right[] = VECTOR(  2,  3,  4,  5,  6,  7,  8,  2 );
static const UINT8 anim_jump_left[]  = VECTOR(  2,  8,  7,  6,  5,  4,  3,  2 );
#define ANIMATION_SPEED_FALL  10
static const UINT8 anim_fall[]       = VECTOR(  9, 10 );

void UpdateMetatile(UINT8 x, UINT8 y, UINT8 id) BANKED;

#define LOOKAHEAD_DISTANCE_PX 192

void CameraLogic(void) {
	scroll_target = NULL;
	UINT8 mask;
	INT16 point;
	if (charge_indicator) SetVisible(charge_indicator, FALSE);
	while (TRUE) {
		if (KEY_PRESSED(J_UP | J_DOWN)) {
			INT16 old_y = scroll_y, dy = 0;
			if KEY_PRESSED(J_UP) {
				mask = (J_A | J_UP), dy = -1;
				point = (THIS->y + 16) - LOOKAHEAD_DISTANCE_PX;
			} else {
				mask = (J_A | J_DOWN ), dy = 1;
				point = THIS->y + (LOOKAHEAD_DISTANCE_PX - DEVICE_SCREEN_PX_HEIGHT);
			}
			while (KEY_PRESSED(mask) == mask) {
				if (scroll_y != point) MoveScroll(scroll_x, scroll_y + dy);
				if ((UINT8)sys_time & 0x01) YIELD;
			}
			while (old_y != scroll_y) {
				MoveScroll(scroll_x, scroll_y - dy);
				if (((UINT8)sys_time & 0x01) == 0) YIELD;
			}
		} else if (KEY_PRESSED(J_LEFT | J_RIGHT)) {
			INT16 old_x = scroll_x, dx = 0;
			if (KEY_PRESSED(J_LEFT)) {
				mask = (J_A | J_LEFT), dx = -1;
				point = (THIS->x + 16) - LOOKAHEAD_DISTANCE_PX;
			} else {
				mask = (J_A | J_RIGHT), dx = 1;
				point = THIS->x + (LOOKAHEAD_DISTANCE_PX - DEVICE_SCREEN_PX_WIDTH);
			}
			while (KEY_PRESSED(mask) == mask) {
				if (scroll_x != point) MoveScroll(scroll_x + dx, scroll_y);
				if ((UINT8)sys_time & 0x01) YIELD;
			}
			while (old_x != scroll_x) {
				MoveScroll(scroll_x - dx, scroll_y);
				if (((UINT8)sys_time & 0x01) == 0) YIELD;
			}
		}
		if KEY_PRESSED(J_A) YIELD; else break;
	}
	if (charge_indicator) SetVisible(charge_indicator, TRUE);
	scroll_target = THIS;
}

void GLUFLogic(void * custom_data) BANKED {
	(void)custom_data;
	UINT8 tile_below;
	UINT8 falling = FALSE;
	UINT8 lifting = TILE_LIFT_NONE;
	UINT8 charge_cooldown;
	UINT8 player_x = start_x, player_y = start_y;
	Sprite * sprite_door = NULL;
	GLUF_charge = charge_cooldown = 0;
	SetSpriteAnim(THIS, anim_enter, ANIMATION_SPEED_ENTER);
	ExecuteSFX(BANK(sfx7exit), sfx7exit, SFX_MUTE_MASK(sfx7exit), SFX_PRIORITY_HIGH);
	for (UINT8 i = 0; i != 42; ++i) {
		YIELD;
	}
	SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
	while (TRUE) {
		if ((!falling) && (!lifting)) {
#ifndef DEBUG_BUILD
			// "lookahead" camera
			if (KEY_PRESSED(J_A)) CameraLogic();
#endif
			// GLUF movements with joypad
			tile_below = level_buffer[player_y + 1][player_x];
			if (KEY_PRESSED(J_UP)) {
				if (check_lift(level_buffer[player_y - 1][player_x])) lifting = TILE_LIFT_UP;
			} else if (KEY_PRESSED(J_DOWN)) {
				if (((check_lift(level_buffer[player_y][player_x]))) && (!check_collision(tile_below))) lifting = TILE_LIFT_DOWN;
				else if (check_lift(tile_below) == TILE_LIFT_DOWN) lifting = TILE_LIFT_DOWN;
			} else if (KEY_PRESSED(J_LEFT)) {
				if (player_x > 0) {
					if (check_collision(level_buffer[player_y][player_x - 1]) == 0) {
						if (tile_below == TILE_DISAPPEARING) {
							UpdateMetatile(player_x, player_y + 1, TILE_DISAPPEARED);
						}
						SetSpriteAnim(THIS, anim_jump_left, ANIMATION_SPEED_JUMP);
						ExecuteSFX(BANK(sfx1jump), sfx1jump, SFX_MUTE_MASK(sfx1jump), SFX_PRIORITY_MINIMAL);
						for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
							THIS->x -= MOVE_SPEED;
							YIELD;
						}
						SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
						player_x--;
					}
				}
			} else if (KEY_PRESSED(J_RIGHT)) {
				if (player_x < (LEVEL_WIDTH - 1)) {
					if (check_collision(level_buffer[player_y][player_x + 1]) == 0) {
						if (tile_below == TILE_DISAPPEARING) {
							UpdateMetatile(player_x, player_y + 1, TILE_DISAPPEARED);
						}
						SetSpriteAnim(THIS, anim_jump_right, ANIMATION_SPEED_JUMP);
						ExecuteSFX(BANK(sfx1jump), sfx1jump, SFX_MUTE_MASK(sfx1jump), SFX_PRIORITY_MINIMAL);
						for (UINT8 i = 0; i != (16 / MOVE_SPEED); ++i) {
							THIS->x += MOVE_SPEED;
							YIELD;
						}
						SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
						player_x++;
					}
				}
			}
		}
		// lifts and physics
		tile_below = level_buffer[player_y + 1][player_x];
		if (lifting == TILE_LIFT_UP) {
			SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
			ExecuteSFX(BANK(sfx4lift), sfx4lift, SFX_MUTE_MASK(sfx4lift), SFX_PRIORITY_MINIMAL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y -= LIFT_SPEED;
				YIELD;
			}
			player_y--;
			switch (check_lift(level_buffer[player_y][player_x])) {
				case TILE_LIFT_NONE:
				case TILE_LIFT_STOP:
					lifting = TILE_LIFT_NONE;
					SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
					break;
				default:
					break;
			}
		} else if (lifting == TILE_LIFT_DOWN) {
			SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
			ExecuteSFX(BANK(sfx4lift), sfx4lift, SFX_MUTE_MASK(sfx4lift), SFX_PRIORITY_MINIMAL);
			for (UINT8 i = 0; i != (16 / LIFT_SPEED); ++i) {
				THIS->y += LIFT_SPEED;
				YIELD;
			}
			if ((!check_lift(tile_below)) || (check_collision(level_buffer[player_y + 2][player_x]))) {
				lifting = TILE_LIFT_NONE;
				SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
			}
			player_y++;
		} else {
			if (player_y < LEVEL_HEIGHT) {
				if (check_collision(tile_below) == 0) {
					falling = TRUE;
					player_y++;
					SetSpriteAnim(THIS, anim_fall, ANIMATION_SPEED_FALL);
					ExecuteSFX(BANK(sfx3fall), sfx3fall, SFX_MUTE_MASK(sfx3fall), SFX_PRIORITY_MINIMAL);
					for (UINT8 i = 0; i != (16 / FALL_SPEED); ++i) {
						THIS->y += FALL_SPEED;
						YIELD;
					}
				} else {
					falling = FALSE;
					SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
				}
			} else {
				ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
				restart = TRUE;
				// kill itself
				return;
			}
		}
		// batteries
		tile_below = level_buffer[player_y + 1][player_x];
		if (tile_below == TILE_BATT_DISCHARGED) {
			if (GLUF_charge) {
				--GLUF_charge;
				UpdateMetatile(player_x, player_y + 1, TILE_BATT_CHARGED);
				if (--battery_count == 0) {
					// activate door
					level_buffer[start_y][start_x] = TILE_DOOR;
					sprite_door = SpriteManagerAdd(SpriteDoor, (start_x << 4) + (TILE_BUFFER_OFFSET << 3), start_y << 4);
				}
				ExecuteSFX(BANK(sfx5paint), sfx5paint, SFX_MUTE_MASK(sfx5paint), SFX_PRIORITY_NORMAL);
			}
		} else if (tile_below == TILE_BATT_CHARGER) {
			if (charge_cooldown) {
				--charge_cooldown;
			} else if (GLUF_charge < CHARGE_MAXIMUM) {
				++GLUF_charge;
				charge_cooldown = CHARGE_COOLDOWN;
				ExecuteSFX(BANK(sfx6noname), sfx6noname, SFX_MUTE_MASK(sfx6noname), SFX_PRIORITY_MINIMAL);
			}
		}
		// exit
		if (level_buffer[player_y][player_x] == TILE_DOOR) {
			if (sprite_door) SpriteManagerRemoveSprite(sprite_door);
			SetAnimationLoop(THIS, FALSE);
			SetSpriteAnim(THIS, anim_exit, ANIMATION_SPEED_ENTER);
			ExecuteSFX(BANK(sfx7exit), sfx7exit, SFX_MUTE_MASK(sfx7exit), SFX_PRIORITY_HIGH);
			for (UINT8 i = 0; i != 42; ++i) {
				YIELD;
			}
			++current_level;
			restart = TRUE;
			// remove ourselves to prevent repeating animations
			SpriteManagerRemoveSprite(THIS);
		}
		YIELD;
	}
}

void GLUFLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	scroll_target = GLUF = NULL;
}

SPRITE_COROUTINE(BANK(SpriteGLUF), GLUFLogic, GLUFLogicFinalizer)
