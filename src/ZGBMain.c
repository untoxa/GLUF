#include <gbdk/platform.h>

#include "Scroll.h"
#include "Sprite.h"
#include "ZGBMain.h"

// game start with the title state
UINT8 next_state = StateTitle;

// we don't need CrossZGB tile replacement, because we handle everything the game code
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	tile_ptr; tile; // suppress warning
	return 255u;    // don't replace anything
}

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
