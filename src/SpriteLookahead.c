// door sprite indicates the exit when it is activated after charging all batteries

#include "Banks/SetAutoBank.h"

#include <gbdk/metasprites.h>

#include "Keys.h"
#include "Sprite.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "GameGlobals.h"

// manually create the invsible metasprite with zero tiles and palettes
metasprite_t lookahead_metasprite[] = { METASPR_TERM };
const metasprite_t* const lookahead_metasprites[1] = { lookahead_metasprite };
const struct MetaSpriteInfo lookahead = {
	.width        = 0,
	.height       = 0,
	.num_tiles    = 0,
	.data         = NULL,
	.num_palettes = 0,
	.palettes     = NULL,
	.num_sprites  = 1,
	.metasprites  = lookahead_metasprites
};
BANKREF(lookahead)

extern Sprite * GLUF;

Sprite * lookahead_camera;

void LookaheadLogic(void * custom_data) BANKED {
	(void)custom_data;
	static INT16 y, old_y, min_y, max_y;
	static INT16 x, old_x, min_x, max_x;

	y = old_y = THIS->y;
	min_y = old_y - LOOKAHEAD_DISTANCE_PX;
	if (min_y < 0) min_y = 0;
	max_y = old_y + (LOOKAHEAD_DISTANCE_PX + 14);

	x = old_x = THIS->x;
	min_x = old_x - LOOKAHEAD_DISTANCE_PX;
	if (min_x < 0) min_x = 0;
	max_x = old_x + (LOOKAHEAD_DISTANCE_PX + 14);

	for (; (KEY_PRESSED(J_A)); YIELD ) {
		INT16 dy = 0, dx = 0;
		UINT8 mask = 0;
		// check GLUF was killed
		if (!GLUF) return;
		if (KEY_PRESSED(J_UP | J_DOWN)) {
			// select up or down lookahead
			if KEY_PRESSED(J_UP) {
				mask = (J_A | J_UP), dy = -LOOKAHEAD_SPEED;
			} else {
				mask = (J_A | J_DOWN ), dy = LOOKAHEAD_SPEED;
			}
		} else if (KEY_PRESSED(J_LEFT | J_RIGHT)) {
			// select left or right lookahead
			if (KEY_PRESSED(J_LEFT)) {
				mask = (J_A | J_LEFT), dx = -LOOKAHEAD_SPEED;
			} else {
				mask = (J_A | J_RIGHT), dx = LOOKAHEAD_SPEED;
			}
		} else {
			continue;
		}
		// while keys are held, try to move camera into direction within the limits
		for (; (KEY_PRESSED(mask) == mask); YIELD) {
			x += dx;
			if (x < min_x) {
				x = min_x;
			} else if (x > max_x) {
				x = max_x;
			}
			THIS->x = (x < 0) ? 0 : x;
			y += dy;
			if (y < min_y) {
				y = min_y;
			} else if (y > max_y) {
				y = max_y;
			}
			THIS->y = (y < 0) ? 0 : y;
			// check GLUF was killed
			if (!GLUF) return;
		}
		// if keys are released, move camera back
		for (; ((x != old_x) || (y != old_y)); YIELD) {
			x -= dx;
			if (dx < 0) {
				if (x > old_x) x = old_x;
			} else {
				if (x < old_x) x = old_x;
			}
			THIS->x = (x < 0) ? 0 : x;
			y -= dy;
			if (dy < 0) {
				if (y > old_y) y = old_y;
			} else {
				if (y < old_y) y = old_y;
			}
			THIS->y = (y < 0) ? 0 : y;
			// check GLUF was killed
			if (!GLUF) return;
		}
	}
}

void LookaheadLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	lookahead_camera = NULL;
}

SPRITE_COROUTINE(LookaheadLogic, LookaheadLogicFinalizer)
