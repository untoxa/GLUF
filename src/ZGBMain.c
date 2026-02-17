#include <gbdk/platform.h>

#include "Scroll.h"
#include "Sprite.h"
#include "ZGBMain.h"

// initialize the game state at start
UINT8 next_state = StateTitle;

// we don't need tile replacement here because this example has no background
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	tile_ptr; tile; // suppress warning
	return 255u;    // don't replace anything
}

/*
// define the custom animation handler
const metasprite_t * GetSpriteAnimation(Sprite* sprite, UINT16 anim_idx) {
	static UINT8 __save;
	const metasprite_t * res;

	__save = CURRENT_BANK;
	SWITCH_ROM(sprite->mt_sprite_bank);
	switch(sprite->type) {
		case SpriteDizzy:
			// load tile data for the Dizzy hardware sprites
			set_sprite_native_data(spriteIdxs[SpriteDizzy], dizzy_TILE_COUNT, dizzy_metatiles[anim_idx]);
			// return address of the metasprite (it is the same for the each animation frame, because we animate tiledata)
			res = dizzy_metasprite0;
			break;
		default:
			res = sprite->mt_sprite_info->metasprites[anim_idx];
			break;
	}
	SWITCH_ROM(__save);
	return res;
}
*/

// we need to override standard scroll clamping function
void ClampScrollLimits(void) {
	if (clamp_enabled) {
		if (scroll_x < SCROLL_LEFT_OFFSET) scroll_x = SCROLL_LEFT_OFFSET;

		if (scroll_x > (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET)) scroll_x = (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET);

		if (scroll_h < (SCREEN_HEIGHT - scroll_h_border)) scroll_y = 0u;
		else if (scroll_y < 0) scroll_y = 0u;

		if (scroll_y > (scroll_h - SCREEN_HEIGHT + scroll_h_border)) scroll_y = (scroll_h - SCREEN_HEIGHT + scroll_h_border);
	}
}
