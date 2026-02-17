#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
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

#define INITIAL_LEVEL_NUMBER 0

extern const struct TilesInfo common_tiles;	// fix png2asset export bug
BANKREF_EXTERN(common_tiles)

#define BANKED_MAP(MAP, TILES) {BANK(MAP), (const UINT8 *)&MAP, BANK(TILES), &TILES}
#define LEVELS_END {0, NULL}

// dynamic level map
UINT8 level_buffer[LEVEL_HEIGHT][LEVEL_WIDTH];
// dynamic map buffer
UINT8 tile_buffer[TILE_BUFFER_HEIGHT * TILE_BUFFER_WIDTH];

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
	.height           = TILE_BUFFER_HEIGHT,
	.attributes       = NULL,
	.tiles_bank       = BANK(common_tiles),
	.tiles            = &common_tiles,
	.extra_tiles_bank = 0,
	.extra_tiles      = NULL
};

// current level
UINT8 current_level;
UINT8 restart;
UINT8 is_title_level;

UINT8 battery_count;

UINT8 teleport_count;
UINT8 teleport_x[MAX_TELEPORTS];
UINT8 teleport_y[MAX_TELEPORTS];

extern Sprite * GLUF;
extern UINT8 start_x, start_y;

void intialize_level_data(UINT8 level);
void spawn_enemies(void);

UINT8 load_level(UINT8 level) {
	// destroy all sprites
	SpriteManagerReset();
	if (!levels[level].map_bank) return FALSE;
	// initialize current level
	intialize_level_data(level);
	// spawn the player sprite
	scroll_target = GLUF = SpriteManagerAdd(SpriteGLUF, (start_x << 4) + (TILE_BUFFER_OFFSET << 3) + 1, (start_y << 4) + 1);
	// spawn enemies
	spawn_enemies();
	// initialize background with collisions (skip the very first tile (19), which is only for the player)
	InitScroll(BANK(StateGame), &current_level_desc, NULL, NULL);
	return TRUE;
}

void spawn_enemies(void) {
	Sprite * enemy;
	// spawn the title
	if (is_title_level = (current_level == 0)) {
		for (UINT8 i = 0; i != 4; ++i) {
			if (enemy = SpriteManagerAdd(SpriteSign, ((i + 4) << 4) + (TILE_BUFFER_OFFSET << 3), (13 << 4))) enemy->custom_data[0] = i;
		}
	} else {
		UINT8 * data = (UINT8 *)level_buffer;
		for (UINT8 y = 0; y != LEVEL_HEIGHT; ++y) {
			for (UINT8 x = 0; x != LEVEL_WIDTH; ++x) {
				switch (*data) {
					case ENEMY_JUMPER:
						enemy = SpriteManagerAdd(SpriteJumper, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
						break;
					case ENEMY_UFO_LEFT:
					case ENEMY_UFO_RIGHT:
					case ENEMY_UFO_UP:
					case ENEMY_UFO_DOWN:
						enemy = SpriteManagerAdd(SpriteUFO, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
						break;
					case ENEMY_JAWS:
						enemy = SpriteManagerAdd(SpriteJaws, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
						break;
					case ENEMY_SLUG:
						enemy = SpriteManagerAdd(SpriteSlug, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
						break;
					case ENEMY_GHOST_LEFT:
					case ENEMY_GHOST_UP:
					case ENEMY_GHOST_DOWN:
					case ENEMY_GHOST_RIGHT:
						enemy = SpriteManagerAdd(SpriteGhost, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
						break;
					default:
						enemy = NULL;
						break;
				}
				if (enemy) {
					enemy->custom_data[0] = *data;
					enemy->custom_data[1] = x;
					enemy->custom_data[2] = y;
				}
				data++;
			}
		}
	}
}

static void set_metatile(UINT8 * ptr, UINT16 id) {
	id = (id << 2) + 1;
	*ptr++ = id++; *ptr = id++;
	ptr += TILE_BUFFER_WIDTH - 1;
	*ptr++ = id++; *ptr = id;
}

void intialize_level_data(UINT8 level) {
	// current level data bank
	static UINT8 id;

	current_level_desc.extra_tiles_bank = levels[level].tiles_bank;
	current_level_desc.extra_tiles = levels[level].tiles;

	battery_count = teleport_count = 0;
	start_x = start_y = 0;

	memset(tile_buffer, 0, sizeof(tile_buffer));
	memcpy_banked(level_buffer, levels[level].map, sizeof(level_buffer), levels[level].map_bank);

	UINT8 * data = (UINT8 *)level_buffer;
	for (UINT8 y = 0; y != LEVEL_HEIGHT; ++y) {
		for (UINT8 x = 0; x != LEVEL_WIDTH; ++x) {
			id = *data;
			switch (id) {
				case TILE_START_POINT:
					start_x = x, start_y = y;
					id = TILE_DOOR;
					break;
				case TILE_BATT_DISCHARGED:
					++battery_count;
					break;
				case TILE_LIFT_STOP:
					id = TILE_LIFT_UP;
					break;
				case ENEMY_JUMPER:
				case ENEMY_UFO_LEFT:
				case ENEMY_UFO_RIGHT:
				case ENEMY_JAWS:
				case ENEMY_GHOST_LEFT:
				case ENEMY_SLUG:
				case ENEMY_UFO_UP:
				case ENEMY_UFO_DOWN:
				case ENEMY_GHOST_UP:
				case ENEMY_GHOST_DOWN:
				case ENEMY_GHOST_RIGHT:
				case MOVE_LEFT:
				case MOVE_RIGHT:
				case MOVE_DOWN:
				case MOVE_UP:
				case MOVE_RIGHT_OR_UP:
				case MOVE_LEFT_OR_UP:
				case MOVE_RIGHT_OR_DOWN:
				case MOVE_LEFT_OR_DOWN:
				case MOVE_ANY_NOT_UP:
				case MOVE_ANY_NOT_DOWN:
				case MOVE_ANY_NOT_RIGHT:
				case MOVE_ANY_NOT_LEFT:
				case MOVE_LEFT_OR_RIGHT:
				case MOVE_UP_OR_DOWN:
				case MOVE_ANY:
					id = (*(data - LEVEL_WIDTH) == TILE_LIFT_UP) ? TILE_LIFT_UP : TILE_EMPTY;
					break;
				case MOVE_GHOST_POINT:
					teleport_x[teleport_count] = x;
					teleport_y[teleport_count] = y;
					++teleport_count;
					id = TILE_EMPTY;
					break;
				default:
					if (id > TILE_LAST_VISIBLE) id = TILE_START_POINT;
					break;
			}
			set_metatile(tile_buffer + ((y << 1) * TILE_BUFFER_WIDTH) + (x << 1) + TILE_BUFFER_OFFSET, id);
			++data;
		}
	}
}

void UpdateMetatile(UINT8 x, UINT8 y, UINT8 id) BANKED {
	level_buffer[y][x] = id;
	y = (y << 1);
	x = (x << 1) + TILE_BUFFER_OFFSET;
	set_metatile(tile_buffer + (y * TILE_BUFFER_WIDTH) + x, id);
	id = (id << 2) + 1;
	x += scroll_offset_x + SCREEN_BKG_OFFSET_X;
	y += scroll_offset_y;
	UpdateMapTile(TARGET_BKG, x,     y,     0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x + 1, y,     0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x,     y + 1, 0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x + 1, y + 1, 0, id,   NULL);
}

void GameLogic(void * custom_data) BANKED {
	(void)custom_data;
	// initialization
	UINT8 skip_press_fire = FALSE;
	// load level
	load_level(current_level = INITIAL_LEVEL_NUMBER);
	YIELD;
	while (TRUE) {
		if (KEY_TICKED(J_A)) {
			if (levels[++current_level].map_bank) skip_press_fire = restart = TRUE; else --current_level;
		} else if (KEY_TICKED(J_B)) {
			if (current_level) {
				--current_level;
				skip_press_fire = restart = TRUE;
			}
		}
		if (restart) {
			restart = FALSE;
			if (!skip_press_fire) {
				// remove "GLUF" sign on the title level
				if (is_title_level) SpriteManagerReset();
				// add "push fire" sign
				SpriteManagerAdd(SpriteFire, 0, 0);
				// wait for pressing A if GLUF was killed
				while ((!KEY_TICKED(J_A)) && (!KEY_TICKED(J_START))) {
					YIELD;
				}
			}
			skip_press_fire = FALSE;
			// fade manually
			FadeIn();
			// reload the level
			if (!load_level(current_level)) break;
			// process engine once before unfade
			YIELD;
			// unfade manually
			FadeOut();
		}
		YIELD;
	}
}

void * game_state_context;

void START(void) {
	scroll_bottom_movement_limit = 100;
	// allocate coroutine context
	coro_runner_process(game_state_context = coro_runner_alloc(GameLogic, BANK(StateGame), NULL));
}

void UPDATE(void) {
	// iterate coroutine
	coro_runner_process(game_state_context);
}

void DESTROY(void) {
	// deallocate coroutine context
	coro_runner_free(game_state_context);
}
