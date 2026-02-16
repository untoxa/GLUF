#ifndef __INCLUDE_LEVELS_H__
#define __INCLUDE_LEVELS_H__

#define LEVEL_HEIGHT 20
#define LEVEL_WIDTH  12
#define LEVEL_METATILE_HEIGHT 2
#define LEVEL_METATILE_WIDTH  2

#define TILE_BUFFER_WIDTH  32
#define TILE_BUFFER_HEIGHT (LEVEL_HEIGHT * LEVEL_METATILE_HEIGHT)
#define TILE_BUFFER_OFFSET ((TILE_BUFFER_WIDTH - ((LEVEL_WIDTH * LEVEL_METATILE_WIDTH))) >> 1)

#define MOVE_SPEED 1
#define FALL_SPEED 2
#define LIFT_SPEED 2

#define TILE_EMPTY           0
#define TILE_START_POINT     4

#define TILE_DOOR            9

#define TILE_LIFT_NONE       TILE_EMPTY
#define TILE_LIFT_UP         5
#define TILE_LIFT_DOWN       10
#define TILE_LIFT_STOP       40

#define TILE_DISAPPEARING    12
#define TILE_DISAPPEARED     3

#define TILE_BATT_CHARGER    11
#define TILE_BATT_DISCHARGED 14
#define TILE_BATT_CHARGED    15

#define TILE_VISIBLE_MASK    0x0f

#define ENEMY_JUMPER         0x20
#define ENEMY_UFO_LEFT       0x21
#define ENEMY_UFO_RIGHT      0x22
#define ENEMY_JAWS           0x23
#define ENEMY_SLUG           0x25
#define ENEMY_UFO_UP         0x26
#define ENEMY_UFO_DOWN       0x27

#define MOVE_LEFT            0x10
#define MOVE_RIGHT           0x11
#define MOVE_DOWN            0x12
#define MOVE_UP              0x13
#define MOVE_RIGHT_OR_UP     0x14
#define MOVE_LEFT_OR_UP      0x15
#define MOVE_RIGHT_OR_DOWN   0x16
#define MOVE_LEFT_OR_DOWN    0x17
#define MOVE_ANY_NOT_DOWN    0x18
#define MOVE_ANY_NOT_UP      0x19
#define MOVE_ANY_NOT_RIGHT   0x1a
#define MOVE_ANY_NOT_LEFT    0x1b
#define MOVE_LEFT_OR_RIGHT   0x1c
#define MOVE_UP_OR_DOWN      0x1d
#define MOVE_ANY             0x1e

#define MOVE_GHOST_POINT     0x1f

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

#endif