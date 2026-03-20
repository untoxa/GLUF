#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE_EX(StateGame)\
_STATE_EX(StateTitle)\
_STATE_EX(StateTitres)\
STATE_DEF_END

#define SPRITES \
_SPRITE(SpriteGLUF,       gluf,       FLIP_NONE)\
_SPRITE(SpriteGhost,      ghost,      FLIP_NONE)\
_SPRITE(SpriteSign,       sign,       FLIP_NONE)\
_SPRITE(SpriteDoor,       door,       FLIP_NONE)\
_SPRITE(SpriteUFO,        ufo,        FLIP_NONE)\
_SPRITE_EX(SpriteSlug,        SpriteSlug,    slug,         FLIP_NONE)\
_SPRITE_EX(SpriteSlugBerserk, SpriteSlug,    slug_berserk, FLIP_NONE)\
_SPRITE(SpriteJumper,     jumper,     FLIP_NONE)\
_SPRITE(SpriteFire,       fire,       FLIP_NONE)\
_SPRITE(SpriteRetrosouls, retrosouls, FLIP_NONE)\
_SPRITE(SpriteTeslafrog,  teslafrog,  FLIP_NONE)\
_SPRITE(SpriteJaws,       jaws,       FLIP_NONE)\
_SPRITE(SpriteIndicator,  indicator,  FLIP_NONE)\
_SPRITE(SpriteTonyandco,  tonyandco,  FLIP_NONE)\
_SPRITE(SpriteLevel,      level,      FLIP_NONE)\
_SPRITE(SpriteBolt,       bolt,       FLIP_NONE)\
_SPRITE_EX(SpriteBattery1,    SpriteBattery, battery1,     FLIP_NONE)\
_SPRITE_EX(SpriteBattery2,    SpriteBattery, battery2,     FLIP_NONE)\
_SPRITE_EX(SpriteBattery3,    SpriteBattery, battery3,     FLIP_NONE)\
_SPRITE_EX(SpriteBattery4,    SpriteBattery, battery4,     FLIP_NONE)\
_SPRITE(SpriteArrow,      arrow,      FLIP_NONE)\
_SPRITE(SpriteSpark,      spark,      FLIP_NONE)\
_SPRITE(SpriteLookahead,  lookahead,  FLIP_NONE)\
_SPRITE(SpriteLiftSpark,  liftspark,  FLIP_NONE)\
_SPRITE(SpriteCode,       code,       FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#define SPRITE_LIMIT_Y 256
#define SPRITE_LIMIT_X 64

#if defined(MASTERSYSTEM)
	#define SCROLL_LEFT_OFFSET  8
	#define SCROLL_RIGHT_OFFSET 0
#else
	#define SCROLL_LEFT_OFFSET  32
	#define SCROLL_RIGHT_OFFSET 32
#endif

#endif