// GLUF is the player character, may walk through the levels, use elevators, fall, charge itself by
// standing on the charger tile, then charge the floor batteries, if collected enough charge

#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "Keys.h"
#include "Sound.h"
#include "Sprite.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

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
#define ANIMATION_SPEED_LIFT  10
static const UINT8 anim_lift[]       = VECTOR( 18, 19 );

// "wobbling batteries" - we must choose the correct sprite, depending on the current level tileset
extern tilesets_e current_tileset;
static const UINT8 battery_sprites[N_TILESETS] = {SpriteBattery1, SpriteBattery2, SpriteBattery3, SpriteBattery4};

void UpdateMetatile(UINT8 x, UINT8 y, UINT8 id) BANKED;

// lookahead logic function
void CameraLogic(void) {
	UINT8 mask;
	INT16 point;
	// reset scroll target, so we can freely move the scroll
	scroll_target = NULL;
	// hide charge indicator
	if (charge_indicator) SetVisible(charge_indicator, FALSE);
	// while A is held
	for (; (KEY_PRESSED(J_A)); YIELD ) {
		if (KEY_PRESSED(J_UP | J_DOWN)) {
			INT16 old_y = scroll_y, dy = 0;
			// select up or down lookahead
			if KEY_PRESSED(J_UP) {
				mask = (J_A | J_UP), dy = -1;
				point = (THIS->y + 16) - LOOKAHEAD_DISTANCE_PX;
				// clamp to minimal Y
				if (point < 0) point = 0;
			} else {
				mask = (J_A | J_DOWN ), dy = 1;
				point = THIS->y + (LOOKAHEAD_DISTANCE_PX - DEVICE_SCREEN_PX_HEIGHT);
				// clamp to maximum Y
				if (point > (scroll_h - SCREEN_HEIGHT + scroll_h_border)) point = (scroll_h - SCREEN_HEIGHT + scroll_h_border);
			}
			// while keys are held, try to move scroll into direction within the limits
			while (KEY_PRESSED(mask) == mask) {
				if (scroll_y != point) MoveScroll(scroll_x, scroll_y + dy);
				if (IS_FRAME_EVEN) YIELD;
			}
			// if keys are released, scroll back
			while (old_y != scroll_y) {
				MoveScroll(scroll_x, scroll_y - dy);
				if (dy < 0 ) {
					if (scroll_y >= (THIS->y - scroll_bottom_movement_limit)) break;
				} else {
					if (scroll_y <= (THIS->y - scroll_top_movement_limit)) break;
				}
				if (IS_FRAME_EVEN) YIELD;
			}
		} else if (KEY_PRESSED(J_LEFT | J_RIGHT)) {
			INT16 old_x = scroll_x, dx = 0;
			// select left or right lookahead
			if (KEY_PRESSED(J_LEFT)) {
				mask = (J_A | J_LEFT), dx = -1;
				point = (THIS->x + 16) - LOOKAHEAD_DISTANCE_PX;
				// clamp to minimal X
				if (point < SCROLL_LEFT_OFFSET) point = SCROLL_LEFT_OFFSET;
			} else {
				mask = (J_A | J_RIGHT), dx = 1;
				point = THIS->x + (LOOKAHEAD_DISTANCE_PX - DEVICE_SCREEN_PX_WIDTH);
				// clamp to maximum X
				if (point > (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET)) point = (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET);
			}
			// while keys are held, try to move scroll into direction within the limits
			while (KEY_PRESSED(mask) == mask) {
				if (scroll_x != point) MoveScroll(scroll_x + dx, scroll_y);
				if (IS_FRAME_EVEN) YIELD;
			}
			// if keys are released, scroll back
			while (old_x != scroll_x) {
				MoveScroll(scroll_x - dx, scroll_y);
				if (IS_FRAME_EVEN) YIELD;
			}
		}
	}
	// show charge indicator
	if (charge_indicator) SetVisible(charge_indicator, TRUE);
	// set scroll target back to player
	scroll_target = THIS;
}

void GLUFLogic(void * custom_data) BANKED {
	(void)custom_data;
	UINT8 tile_below;
	UINT8 falling = FALSE;
	UINT8 lifting = TILE_LIFT_NONE;
	UINT8 charge_cooldown = CHARGE_COOLDOWN;
	UINT8 player_x = start_x, player_y = start_y;
	Sprite * sprite_door = NULL, * sprite_temp;
	// GLUF appears completely discharged
	GLUF_charge = 0;
	// set and play "entering level" animation
	SetAnimationLoop(THIS, FALSE);
	SetSpriteAnim(THIS, anim_enter, ANIMATION_SPEED_ENTER);
	ExecuteSFX(BANK(sfx7exit), sfx7exit, SFX_MUTE_MASK(sfx7exit), SFX_PRIORITY_HIGH);
	DELAY(42);
	// set the idle animation
	SetAnimationLoop(THIS, TRUE);
	SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED_IDLE);
	for (;; YIELD) {
		if ((!falling) && (!lifting)) {
			// lookahead camera logic
			if (KEY_PRESSED(J_A)) CameraLogic();
			// GLUF movements with joypad
			tile_below = level_buffer[player_y + 1][player_x];
			if (KEY_PRESSED(J_UP)) {
				if (check_lift(level_buffer[player_y - 1][player_x])) {
					lifting = TILE_LIFT_UP;
					if (tile_below == TILE_DISAPPEARING) {
						UpdateMetatile(player_x, player_y + 1, TILE_DISAPPEARED);
					}
				}
			} else if (KEY_PRESSED(J_DOWN)) {
				if (((check_lift(level_buffer[player_y][player_x]))) && (!check_collision(tile_below))) lifting = TILE_LIFT_DOWN;
				else if (check_lift(tile_below) == TILE_LIFT_DOWN) lifting = TILE_LIFT_DOWN;
			} else if (KEY_PRESSED(J_LEFT)) {
				if (player_x > 0) {
					if (check_collision(level_buffer[player_y][player_x - 1]) == 0) {
						switch (tile_below) {
							case TILE_DISAPPEARING:
								UpdateMetatile(player_x, player_y + 1, TILE_DISAPPEARED);
								break;
							case TILE_BATT_DISCHARGED:
							case TILE_BATT_CHARGED:
								sprite_temp = SpriteManagerAdd(battery_sprites[current_tileset], THIS->x, THIS->y + 16);
								if (sprite_temp) sprite_temp->custom_data[0] = tile_below;
								break;
							default:
								break;
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
						switch (tile_below) {
							case TILE_DISAPPEARING:
								UpdateMetatile(player_x, player_y + 1, TILE_DISAPPEARED);
								break;
							case TILE_BATT_DISCHARGED:
							case TILE_BATT_CHARGED:
								sprite_temp = SpriteManagerAdd(battery_sprites[current_tileset], THIS->x, THIS->y + 16);
								if (sprite_temp) sprite_temp->custom_data[0] = tile_below;
								break;
							default:
								break;
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
			SetSpriteAnim(THIS, anim_lift, ANIMATION_SPEED_LIFT);
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
			SetSpriteAnim(THIS, anim_lift, ANIMATION_SPEED_LIFT);
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
					if (falling) {
						switch (tile_below) {
							case TILE_BATT_DISCHARGED:
							case TILE_BATT_CHARGED:
								sprite_temp = SpriteManagerAdd(battery_sprites[current_tileset], THIS->x, THIS->y + 16);
								if (sprite_temp) sprite_temp->custom_data[0] = tile_below;
								break;
							default:
								break;
						}
					}
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
				// play GLUF discharging sound
				ExecuteSFX(BANK(sfx5paint), sfx5paint, SFX_MUTE_MASK(sfx5paint), SFX_PRIORITY_NORMAL);
			}
		}
		if (tile_below == TILE_BATT_CHARGER) {
			if (charge_cooldown) {
				--charge_cooldown;
			} else if (GLUF_charge < CHARGE_MAXIMUM) {
				++GLUF_charge;
				charge_cooldown = CHARGE_COOLDOWN;
				// play GLUF charging sound
				ExecuteSFX(BANK(sfx6noname), sfx6noname, SFX_MUTE_MASK(sfx6noname), SFX_PRIORITY_MINIMAL);
				// launch tiny lightning
				SpriteManagerAdd(SpriteBolt, THIS->x, THIS->y);
			}
		} else charge_cooldown = CHARGE_COOLDOWN;
		// exit
		if (level_buffer[player_y][player_x] == TILE_DOOR) {
			// disable collision checking with the enemies while leaving the level
			GLUF = NULL;
			// play "leaving level" animation
			if (sprite_door) SpriteManagerRemoveSprite(sprite_door);
			SetAnimationLoop(THIS, FALSE);
			SetSpriteAnim(THIS, anim_exit, ANIMATION_SPEED_ENTER);
			ExecuteSFX(BANK(sfx7exit), sfx7exit, SFX_MUTE_MASK(sfx7exit), SFX_PRIORITY_HIGH);
			DELAY(42);
			// increase level number, restart level
			++current_level;
			restart = TRUE;
			// remove ourselves
			SpriteManagerRemoveSprite(THIS);
		}
	}
}

void GLUFLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	// reset scroll target and GLUF pointer when killed/removed
	scroll_target = GLUF = NULL;
}

SPRITE_COROUTINE(GLUFLogic, GLUFLogicFinalizer)
