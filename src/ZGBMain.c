#include <gbdk/platform.h>

#include "Scroll.h"
#include "Sprite.h"
#include "ZGBMain.h"

// game start with the title state
UINT8 next_state = StateTitle;

// we don't need CrossZGB tile replacement, because we handle everything in the game code
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	tile_ptr; tile; // suppress warning
	return 255u;    // don't replace anything
}

// we need to override standard scroll clamping CrossZGB function:
// there is the 32-tile width buffer with the level tile map. the actual level is 24 tiles in the middle of that buffer by X

// - on the SMS we need to scroll the screen by X the way, when the maze is centered, considering the left-most column is hidden,
// so we need additional offset of 8px to center it on the TV screen

// - on the GB and GG we need to scroll the level by X, because it does not fit the screen, but scrolling must not go beyond the
// edges of those 24 tiles in the center, so maximum of the level is visible on screen by X

// - nothing special by Y, just copy-paste from the native CrossZGB function
void ClampScrollLimits(void) {
	if (clamp_enabled) {
		if (scroll_x < SCROLL_LEFT_OFFSET) scroll_x = SCROLL_LEFT_OFFSET;

		if (scroll_x > (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET)) scroll_x = (scroll_w - SCREEN_WIDTH - SCROLL_RIGHT_OFFSET);

		if (scroll_h < (SCREEN_HEIGHT - scroll_h_border)) scroll_y = 0u;
		else if (scroll_y < 0) scroll_y = 0u;

		if (scroll_y > (scroll_h - SCREEN_HEIGHT + scroll_h_border)) scroll_y = (scroll_h - SCREEN_HEIGHT + scroll_h_border);
	}
}
