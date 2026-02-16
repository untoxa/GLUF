#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE_EX(StateGame)\
STATE_DEF_END

#define SPRITES \
_SPRITE(SpriteGLUF,   gluf,   FLIP_NONE)\
_SPRITE(SpriteGhost,  ghost,  FLIP_NONE)\
_SPRITE(SpriteSign,   sign,   FLIP_NONE)\
_SPRITE(SpriteDoor,   door,   FLIP_NONE)\
_SPRITE(SpriteUFO,    ufo,    FLIP_NONE)\
_SPRITE(SpriteSlug,   slug,   FLIP_NONE)\
_SPRITE(SpriteJumper, jumper, FLIP_NONE)\
_SPRITE(SpriteFire,   fire,   FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

// tile coodrdinates structure
typedef union position_t {
	struct {
		UINT8 x;
		UINT8 y;
	};
	UINT16 v;
} position_t;

#define IS_ODD_FRAME ((UINT8)sys_time & 1)
#define IS_EVEN_FRAME (!((UINT8)sys_time & 1))

#define SPRITE_LIMIT_Y 256
#define SPRITE_LIMIT_X 64

#endif