// game state - implements the logic of the game; loads levels, spawn sprites, then check conditions for
// restart or switch to the next one. when all levels are beaten, switches itself to the titres state

#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Fade.h"
#include "Vector.h"
#include "Keys.h"
#include "Music.h"
#include "Sound.h"
#include "Coroutines.h"
#include "ZGBMain.h"

#include "bankutils.h"
#include "parallax.h"
#include "GameGlobals.h"

IMPORT_TILES(common_tiles);
IMPORT_TILES(tiles1);
IMPORT_TILES(tiles2);
IMPORT_TILES(tiles3);
IMPORT_TILES(tiles4);

DECLARE_SFX(sfx9teleporting);

DECLARE_MUSIC(music_ingame1);
DECLARE_MUSIC(music_ingame2);
DECLARE_MUSIC(music_outro);

// dynamic level map
UINT8 level_buffer[LEVEL_HEIGHT][LEVEL_WIDTH];
// dynamic map buffer
UINT8 tile_buffer[TILE_BUFFER_HEIGHT * TILE_BUFFER_WIDTH];

// level list structure
typedef struct MapInfoBanked_t {
	UINT8 map_bank;
	const UINT8 * map;
	music_e music;
	tilesets_e tileset;
	UINT16 code;
} MapInfoBanked_t;

#define BANKED_MAP(MAP, MUSIC, TILESET, CODE) {.map_bank = BANK(MAP), .map = (const UINT8 *)&MAP, .music = MUSIC, .tileset = TILESET, .code = CODE}
#define LEVELS_END {.map_bank = 0, .map = NULL, .music = N_MUSICS, .tileset = N_TILESETS, .code = 0}

// tileset list structure
typedef struct TilesetBanked_t {
	UINT8 tiles_bank;
	struct TilesInfo * tiles;
} TilesetBanked_t;

const TilesetBanked_t tilesets[] = {
	[TILESET_1] = {.tiles_bank = BANK(tiles1), .tiles = &tiles1},
	[TILESET_2] = {.tiles_bank = BANK(tiles2), .tiles = &tiles2},
	[TILESET_3] = {.tiles_bank = BANK(tiles3), .tiles = &tiles3},
	[TILESET_4] = {.tiles_bank = BANK(tiles4), .tiles = &tiles4}
};

// level list
const MapInfoBanked_t levels[] = {
	BANKED_MAP(level01, MUSIC_INGAME1, TILESET_1, 0x0000),
	BANKED_MAP(level02, MUSIC_INGAME1, TILESET_1, 0x1741),
	BANKED_MAP(level03, MUSIC_INGAME1, TILESET_1, 0x6803),
	BANKED_MAP(level04, MUSIC_INGAME1, TILESET_1, 0x4751),
	BANKED_MAP(level05, MUSIC_INGAME1, TILESET_1, 0x0674),
	BANKED_MAP(level06, MUSIC_INGAME1, TILESET_1, 0x4361),
	BANKED_MAP(level07, MUSIC_INGAME1, TILESET_1, 0x9503),
	BANKED_MAP(level08, MUSIC_INGAME1, TILESET_1, 0x8335),
	BANKED_MAP(level09, MUSIC_INGAME2, TILESET_2, 0x9004),
	BANKED_MAP(level10, MUSIC_INGAME2, TILESET_2, 0x3935),
	BANKED_MAP(level11, MUSIC_INGAME2, TILESET_2, 0x8661),
	BANKED_MAP(level12, MUSIC_INGAME2, TILESET_2, 0x7312),
	BANKED_MAP(level13, MUSIC_INGAME2, TILESET_2, 0x5957),
	BANKED_MAP(level14, MUSIC_INGAME2, TILESET_2, 0x6626),
	BANKED_MAP(level15, MUSIC_INGAME1, TILESET_3, 0x4902),
	BANKED_MAP(level16, MUSIC_INGAME1, TILESET_3, 0x7393),
	BANKED_MAP(level17, MUSIC_INGAME1, TILESET_3, 0x1082),
	BANKED_MAP(level18, MUSIC_INGAME1, TILESET_3, 0x3430),
	BANKED_MAP(level19, MUSIC_INGAME1, TILESET_3, 0x4498),
	BANKED_MAP(level20, MUSIC_INGAME1, TILESET_3, 0x8659),
	BANKED_MAP(level21, MUSIC_INGAME2, TILESET_4, 0x9741),
	BANKED_MAP(level22, MUSIC_INGAME2, TILESET_4, 0x3952),
	BANKED_MAP(level23, MUSIC_INGAME2, TILESET_4, 0x9615),
	BANKED_MAP(level24, MUSIC_INGAME2, TILESET_4, 0x3744),
	BANKED_MAP(level25, MUSIC_INGAME2, TILESET_4, 0x6442),
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
UINT8 old_level, current_level;

// game control
UINT8 restart;
volatile UINT8 pause;

UINT8 is_title_level;
UINT8 is_cheating;
tilesets_e current_tileset;

UINT8 battery_count;

UINT8 teleport_count;
UINT8 teleport_x[MAX_TELEPORTS];
UINT8 teleport_y[MAX_TELEPORTS];

extern Sprite * GLUF, * charge_indicator;
extern UINT8 start_x, start_y;

void intialize_level_data(UINT8 level);
void spawn_enemies(void);
void set_metatile_priority(UINT8 id);

UINT8 get_level_by_code(UINT16 code) BANKED {
	const MapInfoBanked_t * ptr = levels;
	for (UINT8 i = 0; (ptr->map_bank); ++i, ++ptr ) {
		if (ptr->code == code) return i;
	}
	return 0;
}

void load_music(music_e music) BANKED {
	switch (music) {
		case MUSIC_INGAME1:
			PlayMusic(music_ingame1, 1);
			break;
		case MUSIC_INGAME2:
			PlayMusic(music_ingame2, 1);
			break;
		case MUSIC_OUTRO:
			PlayMusic(music_outro, 1);
			break;
		default:
			StopMusic;
			break;
	}
}

#if defined(ENABLE_CODES)
void show_level_access_code(UINT16 code) {
	// code sprites are 8x16
	SPRITES_8x16;
	// set black background
	memset(tile_buffer, 0, sizeof(tile_buffer));
	// destroy all sprites
	SpriteManagerReset();
	// spawn code
	for (UINT8 i = 0; i != 4; ++i, code >>= 4) {
		UINT8 tx = (((SCREEN_TILES_W - 8) / 2) + 6) - (i << 1);
		UINT8 ty = ((SCREEN_TILES_H - 2) / 2);
		Sprite * sprite = SpriteManagerAdd(SpriteCode, tx << 3, ty << 3);
		sprite->custom_data[0] = SpriteCode;
		sprite->custom_data[1] = tx;
		sprite->custom_data[2] = ty;
		sprite->custom_data[3] = code & 0x0f;
		sprite->custom_data[4] = TRUE;
	}
	// initialize empty background map
	ScrollInitTilesFromMap(0, BANK(StateGame), &current_level_desc);
	ScrollSetMap(BANK(StateGame), &current_level_desc);
	ScrollScreenRedraw();
	// run one engine iteration so everything gets set up
	YIELD;
	// show
	FadeOut();
	// wait any user input
	for (; (!KEY_TICKED(0xff)); YIELD);
	// hide
	FadeIn();
	// set 8x8 sprite mode for the main game
	SPRITES_8x8;
}
#endif

UINT8 load_level(UINT8 level, UINT8 show_code) {
	// disable parallax
	disable_parallax();
	if (!levels[level].map_bank) return FALSE;
#if defined(ENABLE_CODES)
	// show level code and wait for keypress
	if (show_code) show_level_access_code(levels[level].code);
#endif
	// destroy all sprites
	SpriteManagerReset();
	// initialize current level
	intialize_level_data(level);
	// spawn the player sprite
	scroll_target = GLUF = SpriteManagerAdd(SpriteGLUF, (start_x << 4) + (TILE_BUFFER_OFFSET << 3) + 1, (start_y << 4) + 1);
	// spawn charge indicator
	charge_indicator = SpriteManagerAdd(SpriteIndicator, 0, 0);
	// spawn level number
	SpriteManagerAdd(SpriteLevel, 0, 0);
	// spawn enemies
	spawn_enemies();
	// initialize scroll (we don't use InitScroll(), because we need to patch priority for some tiles)
	ScrollInitTilesFromMap(0, BANK(StateGame), &current_level_desc);
	set_metatile_priority(TILE_EMPTY_EXT);
	// initialize scroll map
	ScrollSetMap(BANK(StateGame), &current_level_desc);
	// redraw the screen
	ScrollScreenRedraw();
	// enable parallax and force parallax redraw
	enable_parallax();
	SyncVBlank();
	// level loaded successfully
	return TRUE;
}

void spawn_enemies(void) {
	Sprite * enemy;
	UINT8 * data = (UINT8 *)level_buffer;
	for (UINT8 y = 0; y != LEVEL_HEIGHT; ++y) {
		for (UINT8 x = 0; x != LEVEL_WIDTH; ++x) {
			switch (*data) {
#ifdef ENABLE_ANIMATED_ARROWS
				case TILE_ARROW_DOWN:
				case TILE_ARROW_UP:
					enemy = SpriteManagerAdd(SpriteArrow, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
					break;
#endif
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
				case ENEMY_SLUG_BERSERK:
					enemy = SpriteManagerAdd(SpriteSlugBerserk, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
					break;
				case ENEMY_G:
				case ENEMY_L:
				case ENEMY_U:
				case ENEMY_F:
					enemy = SpriteManagerAdd(SpriteSign, (x << 4) + (TILE_BUFFER_OFFSET << 3), y << 4);
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

// set prioryty of the tiles which are related to the metatile with the specified id
void set_metatile_priority(UINT8 id) {
	id = ID_TO_TILE(id);
	scroll_tile_info[id] |= S_PRIORITY;
	++id;
	scroll_tile_info[id] |= S_PRIORITY;
	++id;
	scroll_tile_info[id] |= S_PRIORITY;
	++id;
	scroll_tile_info[id] |= S_PRIORITY;
}

// update metatile in the tilemap buffer
static void set_metatile(UINT8 * ptr, UINT16 id) {
	id = ID_TO_TILE(id);
	*ptr++ = id;
	*ptr = id + 2;
	ptr += TILE_BUFFER_WIDTH - 1;
	*ptr++ = id + 1;
	*ptr = id + 3;
}

// decode the level data, initialize metatile buffer, tile buffer, etc.
void intialize_level_data(UINT8 level) {
	// current level data bank
	static UINT8 id;

	current_tileset = levels[level].tileset;
	current_level_desc.extra_tiles_bank = tilesets[current_tileset].tiles_bank;
	current_level_desc.extra_tiles = tilesets[current_tileset].tiles;

	// play music
	load_music(levels[level].music);

	battery_count = teleport_count = 0;
	start_x = start_y = 0;

	memset(tile_buffer, 0, sizeof(tile_buffer));
	zx0_decompress_banked(levels[level].map, level_buffer, levels[level].map_bank);

	UINT8 * data = (UINT8 *)level_buffer;
	for (UINT8 y = 0; y != LEVEL_HEIGHT; ++y) {
		for (UINT8 x = 0; x != LEVEL_WIDTH; ++x) {
			id = *data;
			switch (id) {
				case TILE_START_POINT:
					start_x = x, start_y = y;
					id = TILE_DOOR;
					break;
#ifdef ENABLE_ANIMATED_ARROWS
				case TILE_ARROW_DOWN:
				case TILE_ARROW_UP:
					id = TILE_EMPTY;
					break;
#endif
				case TILE_BATT_DISCHARGED:
					++battery_count;
					break;
				case TILE_LIFT_STOP:
					id = TILE_LIFT_UP;
					break;
				case MOVE_GHOST_POINT:
					teleport_x[teleport_count] = x;
					teleport_y[teleport_count] = y;
					++teleport_count;
					id = TILE_EMPTY;
					break;
				default:
					if (id > TILE_LAST_VISIBLE) {
						id = (*(data - LEVEL_WIDTH) == TILE_LIFT_UP) ? TILE_LIFT_UP : TILE_EMPTY;
					}
					break;
			}
			set_metatile(tile_buffer + ((y << 1) * TILE_BUFFER_WIDTH) + (x << 1) + TILE_BUFFER_OFFSET, id);
			++data;
		}
	}
}

// update metatile in the game level buffer, in the level tilemap and on screen
void UpdateMetatile(UINT8 x, UINT8 y, UINT8 id) BANKED {
	level_buffer[y][x] = id;
	y = (y << 1);
	x = (x << 1) + TILE_BUFFER_OFFSET;
	set_metatile(tile_buffer + (y * TILE_BUFFER_WIDTH) + x, id);
	x += scroll_offset_x + SCREEN_BKG_OFFSET_X;
	y += scroll_offset_y;
	id = ID_TO_TILE(id);
	UpdateMapTile(TARGET_BKG, x,     y,     0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x,     y + 1, 0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x + 1, y,     0, id++, NULL);
	UpdateMapTile(TARGET_BKG, x + 1, y + 1, 0, id,   NULL);
}

NORETURN void GameLogic(void * custom_data) BANKED {
	(void)custom_data;
	// initialization
	volatile UINT8 skip_press_fire = FALSE;
	// set up CrossZGB scrolling parameters
# if defined(MASTERSYSTEM)
	scroll_top_movement_limit = 56;
	scroll_bottom_movement_limit = 120;
#else
	scroll_top_movement_limit = 40;
	scroll_bottom_movement_limit = 88;
#endif
	ENABLE_SCROLL_CLAMPING;

	// load level
	old_level = current_level;
	load_level(current_level, FALSE);
	// process once to exit the state INIT()
	YIELD;

	for (;; YIELD) {
#ifdef ENABLE_CHEATS
		// if cheating - allow change levels with B + UP/DOWN, including title and titres (no-return) states
		if ((is_cheating) && (KEY_PRESSED(J_B))) {
			if (KEY_TICKED(J_UP)) {
				// switch to the next level, if no more then run titres state
				if (levels[++current_level].map_bank) {
					// prevent showing code
					old_level = current_level;
					// signal restart
					skip_press_fire = restart = TRUE;
				} else SetState(StateTitres);
			} else if (KEY_TICKED(J_DOWN)) {
				// switch to the previous level if not zero, else run title state
				if (current_level) {
					--current_level;
					// prevent showing code
					old_level = current_level;
					// signal restart
					skip_press_fire = restart = TRUE;
				} else SetState(StateTitle);
			}
		}
#endif
		if (restart) {
			restart = FALSE;
			if (!skip_press_fire) {
				// disable sprite flickering, so "press fire" stay on top
				DISABLE_SPRITE_FLICKERING;
				// add "push fire" sign
				SpriteManagerBringToFront(SpriteManagerAdd(SpriteFire, 0, 0));
				// wait for pressing A if GLUF was killed, keep flickering sprites manually
				for (; (!KEY_TICKED(FIRE_BUTTONS)); YIELD) VectorRotateFrom(sprite_manager_updatables, 1);
				// enable sprite flickering
				ENABLE_SPRITE_FLICKERING;
			} else skip_press_fire = FALSE;
			// play level change SFX
			ExecuteSFX(BANK(sfx9teleporting), sfx9teleporting, SFX_MUTE_MASK(sfx9teleporting), SFX_PRIORITY_NORMAL);
			// fade manually
			FadeIn();
			// reload the level
			if (!load_level(current_level, (old_level != current_level))) SetState(StateTitres);
			old_level = current_level;
			// process engine once before unfade
			YIELD;
			// unfade manually
			FadeOut();
		}
#if !defined(MASTERSYSTEM)
		else if (KEY_TICKED(J_START)) pause = TRUE;
#endif
		if (pause) {
			// stop SFX if playing, pause the music
			sfx_reset_sample();
			MuteMusicChannels(MUTE_MASK_NONE);
			PauseMusic;
			sfx_sound_cut();
			// wait until unpause (different for the SMS)
#if defined(MASTERSYSTEM)
			while (pause) vsync();
#else
			waitpadup();
			waitpad(J_START);
			pause = FALSE;
#endif
			// resume the music playback
			ResumeMusic;
		}
	}
}

void GameLogicFinalizer(void * custom_data) BANKED {
	(void)custom_data;
	// disable parallax on state change
	disable_parallax();
}

STATE_COROUTINE(GameLogic, GameLogicFinalizer)
