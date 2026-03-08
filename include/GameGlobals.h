#ifndef __INCLUDE_GAME_GLOBALS_H__
#define __INCLUDE_GAME_GLOBALS_H__

#include <gbdk/platform.h>

#include <rand.h>
#include <stdbool.h>

#include "Coroutines.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Sound.h"

#if defined(MASTERSYSTEM)
	#define START_BUTTONS (J_A)
#else
	#define START_BUTTONS (J_A | J_START)
#endif

#define ENABLE_CHEATS
#define ENABLE_PARALLAX
#if defined(NINTENDO)
	#define ENABLE_ANIMATED_ARROWS
#endif

#define INITIAL_LEVEL_NUMBER    0

#define LEVEL_HEIGHT            20
#define LEVEL_WIDTH             12
#define LEVEL_METATILE_HEIGHT   2
#define LEVEL_METATILE_WIDTH    2

#define TILE_BUFFER_WIDTH       32
#define TILE_BUFFER_HEIGHT      (LEVEL_HEIGHT * LEVEL_METATILE_HEIGHT)
#define TILE_BUFFER_OFFSET      ((TILE_BUFFER_WIDTH - ((LEVEL_WIDTH * LEVEL_METATILE_WIDTH))) >> 1)

#define MOVE_SPEED              1
#define FALL_SPEED              2
#define LIFT_SPEED              2

#define TILE_EMPTY              0x00
#define TILE_EMPTY_EXT          0x02
#define TILE_START_POINT        0x04

#define TILE_ARROW_DOWN		0x07
#define TILE_ARROW_UP		0x08

#define TILE_DOOR               0x09

#define TILE_LIFT_NONE          TILE_EMPTY
#define TILE_LIFT_UP            0x05
#define TILE_LIFT_DOWN          0x0a
#define TILE_LIFT_STOP          0x28

#define TILE_DISAPPEARING       0x0c
#define TILE_DISAPPEARED        0x03

#define TILE_BATT_CHARGER       0x0b
#define TILE_BATT_DISCHARGED    0x0e
#define TILE_BATT_CHARGED       0x0f

#define ENEMY_JUMPER            0x20
#define ENEMY_UFO_LEFT          0x21
#define ENEMY_UFO_RIGHT         0x22
#define ENEMY_JAWS              0x23
#define ENEMY_GHOST_LEFT        0x24
#define ENEMY_SLUG              0x25
#define ENEMY_UFO_UP            0x26
#define ENEMY_UFO_DOWN          0x27
#define ENEMY_GHOST_UP	        0x29
#define ENEMY_GHOST_DOWN        0x2a
#define ENEMY_GHOST_RIGHT       0x2b

#define MOVE_LEFT               0x10
#define MOVE_RIGHT              0x11
#define MOVE_DOWN               0x12
#define MOVE_UP                 0x13
#define MOVE_RIGHT_OR_UP        0x14
#define MOVE_LEFT_OR_UP         0x15
#define MOVE_RIGHT_OR_DOWN      0x16
#define MOVE_LEFT_OR_DOWN       0x17
#define MOVE_ANY_NOT_DOWN       0x18
#define MOVE_ANY_NOT_UP         0x19
#define MOVE_ANY_NOT_RIGHT      0x1a
#define MOVE_ANY_NOT_LEFT       0x1b
#define MOVE_LEFT_OR_RIGHT      0x1c
#define MOVE_UP_OR_DOWN         0x1d
#define MOVE_ANY                0x1e
#define MOVE_GHOST_POINT        0x1f

#define MAX_TELEPORTS		0x20

typedef enum enemy_dir_e {
	DIR_NONE = 0,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	N_DIRECTIONS
} enemy_dir_e;

#define TILE_FIRST_SOLID     TILE_LIFT_DOWN
#define TILE_LAST_VISIBLE    TILE_BATT_CHARGED

typedef enum {
	MUSIC_INGAME1  = 0,
	MUSIC_INGAME2,
	MUSIC_OUTRO,
	N_MUSICS,
	MUSIC_INTRO   = MUSIC_INGAME1,
} music_e;

typedef enum {
	TILESET_1  = 0,
	TILESET_2,
	TILESET_3,
	TILESET_4,
	N_TILESETS
} tilesets_e;

extern UINT8 level_buffer[LEVEL_HEIGHT][LEVEL_WIDTH];

#define EXTERN_DATA(A) BANKREF_EXTERN(A)\
extern void A;

EXTERN_DATA(level01)
EXTERN_DATA(level02)
EXTERN_DATA(level03)
EXTERN_DATA(level04)
EXTERN_DATA(level05)
EXTERN_DATA(level06)
EXTERN_DATA(level07)
EXTERN_DATA(level08)
EXTERN_DATA(level09)
EXTERN_DATA(level10)
EXTERN_DATA(level11)
EXTERN_DATA(level12)
EXTERN_DATA(level13)
EXTERN_DATA(level14)
EXTERN_DATA(level15)
EXTERN_DATA(level16)
EXTERN_DATA(level17)
EXTERN_DATA(level18)
EXTERN_DATA(level19)
EXTERN_DATA(level20)
EXTERN_DATA(level21)
EXTERN_DATA(level22)
EXTERN_DATA(level23)
EXTERN_DATA(level24)
EXTERN_DATA(level25)

inline UINT8 check_collision(UINT8 id) {
	if ((id < TILE_FIRST_SOLID) || (id > TILE_LAST_VISIBLE)) return TILE_EMPTY; else return id;
}
inline UINT8 check_lift(UINT8 id) {
	if ((id == TILE_LIFT_UP) || (id == TILE_LIFT_DOWN) || (id == TILE_LIFT_STOP)) return id; else return TILE_LIFT_NONE;
}

extern Sprite * GLUF, * charge_indicator;
extern UINT8 restart;

DECLARE_SFX(sfx10dead_nonoise);

inline void CheckKillGLUF(Sprite * sprite) {
	if ((GLUF) && (CheckCollision(sprite, GLUF))) {
		ExecuteSFX(BANK(sfx10dead_nonoise), sfx10dead_nonoise, SFX_MUTE_MASK(sfx10dead_nonoise), SFX_PRIORITY_HIGH);
		SpriteManagerRemoveSprite(GLUF);
		if (charge_indicator) SpriteManagerRemoveSprite(charge_indicator);
		scroll_target = NULL;
		restart = TRUE;
	}
}

inline void CompensateScroll(void) {
#ifdef MASTERSYSTEM
	MoveScroll(8, 0);
#endif
}

inline bool chance_25_percent(void) {
	return ((rand() & 0x03) == 0);
}
inline bool chance_50_percent(void) {
	return (rand() & 0x01);
}
inline bool chance_75_percent(void) {
	return (rand() & 0x03);
}

#define ARRAY_PICK_RANDOM(ARRAY) (ARRAY[(rand() % sizeof(ARRAY))])
#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

void load_music(music_e music) BANKED;

#define SECONDS(A) ((A) * 60)

#if defined(NINTENDO)
#define BPP_SHIFT 1
#else
#define BPP_SHIFT 2
#endif
#define TILE_SIZE_BYTES (8 << BPP_SHIFT)
#define ID_TO_TILE(id) (((id) << 2) + 1)

#define LOOKAHEAD_DISTANCE_PX 192

inline void DELAY(UINT16 frames) {
	for (; (frames); --frames) YIELD;
}

#define IS_FRAME_ODD ((UINT8)sys_time & 0x01u)
#define IS_FRAME_EVEN (((UINT8)sys_time & 0x01u) == 0)

#endif