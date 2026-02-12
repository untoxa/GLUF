#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "SpriteManager.h"
#include "Fade.h"
#include "Vector.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Coroutines.h"

#include "bankutils.h"
#include "levels.h"
#include "tiles1.h"
#include "tiles2.h"
#include "tiles3.h"
#include "tiles4.h"

extern const struct TilesInfo common_tiles;	// fix png2asset export bug
BANKREF_EXTERN(common_tiles)

#define BANKED_MAP(MAP, TILES) {BANK(MAP), (const UINT8 *)&MAP, BANK(TILES), &TILES}
#define LEVELS_END {0, NULL}

#define TILE_BUFFER_WIDTH 32
#define TILE_BUFFER_OFFSET ((TILE_BUFFER_WIDTH - ((LEVEL_WIDTH * LEVEL_METATILE_WIDTH))) >> 1)

// dynamic level map
UINT8 level_buffer[LEVEL_HEIGHT][LEVEL_WIDTH];
// dynamic map buffer
UINT8 tile_buffer[(LEVEL_HEIGHT * LEVEL_METATILE_HEIGHT) * TILE_BUFFER_WIDTH];

// level list structure
typedef struct MapInfoBanked_t {
	UINT8 map_bank;
	const UINT8 * map;
	UINT8 tiles_bank;
	struct TilesInfo * tiles;
} MapInfoBanked_t;

// level list
const MapInfoBanked_t levels[] = {
	BANKED_MAP(level01, tiles1),
	BANKED_MAP(level02, tiles1),
	BANKED_MAP(level03, tiles1),
	BANKED_MAP(level04, tiles1),
	BANKED_MAP(level05, tiles1),
	BANKED_MAP(level06, tiles1),
	BANKED_MAP(level07, tiles1),
	BANKED_MAP(level08, tiles2),
	BANKED_MAP(level09, tiles2),
	BANKED_MAP(level10, tiles2),
	BANKED_MAP(level11, tiles2),
	BANKED_MAP(level12, tiles2),
	BANKED_MAP(level13, tiles2),
	BANKED_MAP(level14, tiles3),
	BANKED_MAP(level15, tiles3),
	BANKED_MAP(level16, tiles3),
	BANKED_MAP(level17, tiles3),
	BANKED_MAP(level18, tiles3),
	BANKED_MAP(level19, tiles3),
	BANKED_MAP(level20, tiles4),
	BANKED_MAP(level21, tiles4),
	BANKED_MAP(level22, tiles4),
	BANKED_MAP(level23, tiles4),
	BANKED_MAP(level24, tiles4),
	BANKED_MAP(level25, tiles4),
	LEVELS_END
};

// current level structures
struct MapInfo current_level_desc = {
	.data             = tile_buffer,
	.width            = TILE_BUFFER_WIDTH,
	.height           = (LEVEL_HEIGHT * LEVEL_METATILE_HEIGHT),
	.attributes       = NULL,
	.tiles_bank       = BANK(common_tiles),
	.tiles            = &common_tiles,
	.extra_tiles_bank = 0,
	.extra_tiles      = NULL
};

// current level
UINT8 current_level;
UINT8 restart;
// score
UINT16 game_score;

UINT8 battery_count;

void intialize_level_data(UINT8 level);

extern Sprite * GLUF;
extern UINT8 player_x, player_y;

UINT8 load_level(UINT8 level) {
	// destroy all sprites
	SpriteManagerReset();
	if (!levels[level].map_bank) return FALSE;
	// initialize current level
	intialize_level_data(level);
	// spawn the player sprite
	scroll_target = GLUF = SpriteManagerAdd(SpriteGLUF, (player_x << 4) + (TILE_BUFFER_OFFSET << 3), player_y << 4);
	// initialize background with collisions (skip the very first tile (19), which is only for the player)
	InitScroll(BANK(StateGame), &current_level_desc, NULL, NULL);
	return TRUE;
}

static void set_metatile(UINT8 * ptr, UINT16 id) {
	id = (id << 2) + 1;
	*ptr++ = id++; *ptr = id++;
	ptr += TILE_BUFFER_WIDTH - 1;
	*ptr++ = id++; *ptr = id;
}

void intialize_level_data(UINT8 level) {
	// current level data bank
	static UINT8 __save;

	current_level_desc.extra_tiles_bank = levels[level].tiles_bank;
	current_level_desc.extra_tiles = levels[level].tiles;

	battery_count = 0;
	player_x = player_y = 0;

	memset(tile_buffer, 0, sizeof(tile_buffer));
	memcpy_banked(level_buffer, levels[level].map, sizeof(level_buffer), levels[level].map_bank);

	UINT8 * data = (UINT8 *)level_buffer;
	for (UINT8 y = 0; y != LEVEL_HEIGHT; ++y) {
		for (UINT8 x = 0; x != LEVEL_WIDTH; ++x) {
			switch (*data) {
				case 4:
					*data = 0;
					player_x = x, player_y = y;
					break;
				default:
					if (*data >= 16) *data = 0;
					break;
			}
			set_metatile(tile_buffer + ((y << 1) * TILE_BUFFER_WIDTH) + (x << 1) + TILE_BUFFER_OFFSET, *data);
			++data;
		}
	}
}

void GameLogic(void * custom_data) BANKED {
	(void)custom_data;
	// initialization
	game_score = 0;
	// load level
	FadeIn();
	load_level(current_level = 0);
	YIELD;
	FadeOut();
	while (TRUE) {
		if (restart) {
			restart = FALSE;
			FadeIn();
			if (!load_level(current_level)) return;
			YIELD;
			FadeOut();
		}
		if (KEY_TICKED(J_A)) {
			FadeIn();
			if (!load_level(++current_level)) return;
			YIELD;
			FadeOut();
		}
		YIELD;
	}
}

void * game_state_context;

void START(void) {
	fade_mode = FADE_MANUAL;
	// allocate coroutine context
	game_state_context = coro_runner_alloc(GameLogic, BANK(StateGame), NULL);
}

void UPDATE(void) {
	// iterate coroutine
	coro_runner_process(game_state_context);
}

void DESTROY(void) {
	// deallocate coroutine context
	coro_runner_free(game_state_context);
	fade_mode = FADE_DEFAULT;
}
