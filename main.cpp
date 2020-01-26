/*
Max Huang
Mrs Cullum
June 12th, 2017
Danniponcho - the Quest for Hot Sauce

This program is the code for the game Danniponcho - the Quest for Hot Sauce.

Known bugs:
 - None
*/


// regular libraries
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// allegro libraries
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


// system variables
#define WINDOW_TITLE      "Danniponcho - the Quest for Hot Sauce"
#define WINDOW_ICON       al_load_bitmap("Image/System/icon.png")
#define FPS               30
#define GAME_WIDTH        640
#define GAME_HEIGHT       480
#define FULLSCREEN        false
#define HIDE_MOUSE        false
#define CONSOLE_ENABLED   true
#define DEXTAR            false
#define MAX_ARR_SIZE      5000

#define TILESET_WIDTH       8
#define PARALLAX_OPACITY    0.8
#define EVENT_LOG_MAX_SIZE  4

// keyboard controls
#define KEY_UP         ALLEGRO_KEY_UP
#define KEY_DOWN       ALLEGRO_KEY_DOWN
#define KEY_LEFT       ALLEGRO_KEY_LEFT
#define KEY_RIGHT      ALLEGRO_KEY_RIGHT
#define KEY_DASH       ALLEGRO_KEY_LSHIFT
#define KEY_ATTACK     ALLEGRO_KEY_SPACE
#define KEY_SELECT     ALLEGRO_KEY_Z
#define KEY_CANCEL     ALLEGRO_KEY_X
#define KEY_ITEM       ALLEGRO_KEY_C

// volume variables
#define MASTER_VOLUME  0.5
#define BGM_VOLUME     1.0    * MASTER_VOLUME
#define SE_VOLUME      0.8    * MASTER_VOLUME

/******************************************************
* -----------------------------------------------------
* --------------- DO NOT EDIT BELOW   -----------------
* -----------------------------------------------------
******************************************************/

// directions
#define DOWN           0
#define LEFT           1
#define RIGHT          2
#define UP             3

// object numbers
#define TILESET_NUM           1
#define WEAPON_SOUND_NUM      3
#define MENU_SELECTION_NUM    7
#define TITLE_SELECTION_NUM   4

// level 1
#define ENEMY_NUM_1           3
#define ITEM_NUM_1            0
#define FOUNTAIN_NUM_1        0

// level 2
#define ENEMY_NUM_2           10
#define ITEM_NUM_2            0
#define FOUNTAIN_NUM_2        1

// level 3
#define ENEMY_NUM_3           11
#define ITEM_NUM_3            2
#define FOUNTAIN_NUM_3        1

// weapon types
#define WEAPON_MELEE   0
#define WEAPON_RANGE   1

// character types
#define PLAYER         0
#define ENEMY          1
#define BOSS           2

// item types
#define WEAPON         0
#define ITEM           1

// character types
#define C_PLAYER         0
#define C_DUMMY          1
#define C_OBJECT         2
#define C_PIRATE         3
#define C_TELEMARKETER   4
#define C_YIRAN          5

// weapons
#define WEAPON_SIZE    3
#define STICK          0
#define SWORD          1
#define CELLO          2

// items
#define ITEM_SIZE      2
#define NO_ITEM        0
#define POTION         1

// scenes
#define GAME           0
#define MENU           1
#define TITLE          2
#define CREDITS        3
#define INSTRUCTIONS   4
#define MENU_CONTROLS  5

// weapon struct
struct Weapon {

	// images
	ALLEGRO_BITMAP *spritesheet = NULL;
	ALLEGRO_BITMAP *rangePic = NULL;

	// weapon type (melee, range, ...)
	int type;
	int name;

	// sound effect
	ALLEGRO_SAMPLE *se[WEAPON_SOUND_NUM];
	int seId = 0;

	// animation
	int frameX = 0, frameY = 0;
	int animFreq = 4, animFreqCheck = 999;

	// pwoer
	int damage = 0;

	// range (if range weapon)
	float rangeDist = 2.5;

};
Weapon new_weapon(int t, int wN, ALLEGRO_BITMAP *s, ALLEGRO_BITMAP *r, int d, ALLEGRO_SAMPLE *se[]);

// item struct
struct Item {

	// info
	int type;
	int id;
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_SAMPLE *se = al_load_sample("Audio/Se/se_event.ogg");

	// loacation
	int x, y;

};
Item new_item(int t, int id, ALLEGRO_BITMAP *img, int x, int y);

// fountain ("shop") struct
struct Fountain {

	// info
	int x, y;
	ALLEGRO_BITMAP *image_active = NULL;
	ALLEGRO_BITMAP *image_inactive = NULL;
	ALLEGRO_BITMAP *balloonPopupAble = al_load_bitmap("Image/Balloon/balloon_z.png");
	ALLEGRO_BITMAP *balloonPopupInable = al_load_bitmap("Image/Balloon/balloon_noCoin.png");
	ALLEGRO_SAMPLE *se = al_load_sample("Audio/Se/se_summon.ogg");

	// cost and item drop
	int cost;
	float weaponChance;
	float itemChance;

	// reload
	int reloadFreq = 180, reloadFreqCheck = 999;

};
Fountain new_fountain(ALLEGRO_BITMAP *iA, ALLEGRO_BITMAP *iI, int c, int x, int y, float wC, float iC);

//inventory (player items, gold) struct
struct Inventory {

	// inventory
	int gold = 0;
	int potions = 0;

	// potion recharge time
	int potionFreq = 300, potionFreqCheck = 999;

	// potion popup
	ALLEGRO_BITMAP *potionPopup = al_load_bitmap("Image/Balloon/balloon_c.png");

};

// character struct (player and enemies)
struct Character {

	// character type
	int type;
	int characterType;

	// image
	ALLEGRO_BITMAP *spritesheet = NULL;
	ALLEGRO_BITMAP *balloonPopup = NULL;
	bool popup = false;

	// sound effects
	ALLEGRO_SAMPLE *se_damage = al_load_sample("audio/Se/se_damage.ogg");
	ALLEGRO_SAMPLE *se_heal = al_load_sample("audio/Se/se_heal.ogg");

	// position and animation
	int x = 0, y = 0;
	int dir = 0;

	// animation
	int frameX = 1, frameY = 0;
	int animFreq = 7, animFreqCheck = 999;

	// speed
	int stepSize = 8;
	float dashMultiplier = 1.5;

	// hit buffer
	int hitBuff = 20, hitBuffCheck = 999;

	//state
	bool moving = false, dashing = false;
	bool attacking = false, invincible = false;
	bool dead = false;

	// enemy variables <begin>
	float followRange = 4;
	int atkFreq = 45, atkFreqCheck = 999;
	int atkRand = 15;
	int itemDrop = NO_ITEM;
	int yMin = 0, yMax = 999;
	ALLEGRO_BITMAP *enemyPopup = al_load_bitmap("Image/Balloon/balloon_space.png");
	// enemy variables <end>

	// stats
	float hp = 0, maxHp = 0;
	float mp = 0, maxMp = 0, mpRegen = 0.2;
	int atk = 0;

	// "items"
	Weapon weapon;
	Inventory inventory = Inventory();

};
Character new_character(int t, int cT, ALLEGRO_BITMAP *s, int x, int y, int dir, int h, int m, int a, Weapon w, int minY, int maxY);

// hud struct (hud, menu, popups)
struct Hud {

	// hud images
	ALLEGRO_BITMAP *hud_back = al_load_bitmap("Image/Hud/Hud/hud_back.png");
	ALLEGRO_BITMAP *hud_hpBar = al_load_bitmap("Image/Hud/Hud/hud_hp.png");
	ALLEGRO_BITMAP *hud_mpBar = al_load_bitmap("Image/Hud/Hud/hud_mp.png");
	ALLEGRO_BITMAP *hud_info = al_load_bitmap("Image/Hud/Hud/hud_info.png");
	ALLEGRO_BITMAP *hud_eventLog = al_load_bitmap("Image/Hud/Hud/hud_eventLog.png");

	// popup images
	ALLEGRO_BITMAP *text_gameover = al_load_bitmap("Image/Hud/Text/text_gameOver.png");
	ALLEGRO_BITMAP *text_win = al_load_bitmap("Image/Hud/Text/text_win.png");
	ALLEGRO_BITMAP *text_console = al_load_bitmap("Image/Hud/Text/text_console.png");
	ALLEGRO_BITMAP *text_bossFight = al_load_bitmap("Image/Hud/Text/text_bossFight.png");

	// menu images
	ALLEGRO_BITMAP *menu_back = al_load_bitmap("Image/Hud/Menu/menu_back.png");
	ALLEGRO_BITMAP *menu_music = al_load_bitmap("Image/Hud/Menu/menu_musicSelected.png");
	ALLEGRO_BITMAP *menu_sound = al_load_bitmap("Image/Hud/Menu/menu_soundSelected.png");
	ALLEGRO_BITMAP *menu_controls = al_load_bitmap("Image/Hud/Menu/menu_controlsSelected.png");
	ALLEGRO_BITMAP *menu_framerate = al_load_bitmap("Image/Hud/Menu/menu_framerateSelected.png");
	ALLEGRO_BITMAP *menu_cancel = al_load_bitmap("Image/Hud/Menu/menu_cancelSelected.png");
	ALLEGRO_BITMAP *menu_restart = al_load_bitmap("Image/Hud/Menu/menu_restartSelected.png");
	ALLEGRO_BITMAP *menu_exit = al_load_bitmap("Image/Hud/Menu/menu_exitSelected.png");

	// title images
	ALLEGRO_BITMAP *title_back = al_load_bitmap("Image/Hud/Title/title_back.png");
	ALLEGRO_BITMAP *title_title = al_load_bitmap("Image/Hud/Title/title_title.png");
	ALLEGRO_BITMAP *title_play = al_load_bitmap("Image/Hud/Title/title_playSelected.png");
	ALLEGRO_BITMAP *title_instructions = al_load_bitmap("Image/Hud/Title/title_instructionsSelected.png");
	ALLEGRO_BITMAP *title_credits = al_load_bitmap("Image/Hud/Title/title_creditsSelected.png");
	ALLEGRO_BITMAP *title_exit = al_load_bitmap("Image/Hud/Title/title_exitSelected.png");
	ALLEGRO_BITMAP *credits = al_load_bitmap("Image/Hud/Title/title_credits.png");
	ALLEGRO_BITMAP *instructions = al_load_bitmap("Image/Hud/Title/title_instructions.png");

	// arrow images
	ALLEGRO_BITMAP *arrow_up = al_load_bitmap("Image/Hud/Arrow/arrow_up.png");
	ALLEGRO_BITMAP *arrow_down = al_load_bitmap("Image/Hud/Arrow/arrow_down.png");

	// sound effects
	ALLEGRO_SAMPLE *se_select = al_load_sample("Audio/Se/se_select.ogg");
	ALLEGRO_SAMPLE *se_cancel = al_load_sample("Audio/Se/se_cancel.ogg");
	ALLEGRO_SAMPLE *se_scroll = al_load_sample("Audio/Se/se_scroll.ogg");

	// fonts
	ALLEGRO_FONT *font_hp = al_load_ttf_font("Font/metal_lord.ttf", 18, 0);
	ALLEGRO_FONT *font_mp = al_load_ttf_font("Font/metal_lord.ttf", 14, 0);
	ALLEGRO_FONT *font_potions = al_load_ttf_font("Font/metal_lord.ttf", 18, 0);
	ALLEGRO_FONT *font_gold = al_load_ttf_font("Font/metal_lord.ttf", 24, 0);
	ALLEGRO_FONT *font_framerate = al_load_ttf_font("Font/metal_lord.ttf", 24, 0);
	ALLEGRO_FONT *font_eventLog = al_load_ttf_font("Font/metal_lord.ttf", 14, 0);

	// popup
	int popupDur = 90, popupDurCheck = 999;
	bool bossFightPopup = false;

	// index selection
	int indexSelected = 0;
	int indexSelectFreq = 5, indexSelectFreqCheck = 999;
	int indexEnterFreq = 5, indexEnterFreqCheck = 999;
	int indexToggleFreq = 5, indexToggleFreqCheck = 999;

	int creditsY = 0;
	int creditsScrollrate = 30;

	// menu options
	bool music = true;
	bool sound = true;
	bool framerate = false;

	// framerate update frequency
	int framerateFreq = 4, framerateFreqCheck = 999;

	// event log
	char eventLog[EVENT_LOG_MAX_SIZE][100] = { "", "", "", "" };
	int eventLogColor[EVENT_LOG_MAX_SIZE][3] = {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
	};
	int eventLogSize = 0;
	int eventLogFadeFreq = 180, eventLogFadeFreqCheck = 999;

};

// camera struct
struct Camera {

	// info
	int x = 0, y = 0;
	bool freeze = false;

};

// tileset struct
struct Tileset {

	// image
	ALLEGRO_BITMAP *image = NULL;

	// data
	int w = 4, h = 0;
	int data[MAX_ARR_SIZE];

	// animation
	int frame = 0;
	int animFreq = 30, animFreqCheck = 999;


};
Tileset new_tileset(ALLEGRO_BITMAP *i, FILE *d);

// parallax struct
struct Parallax {

	// images
	ALLEGRO_BITMAP *img1;
	ALLEGRO_BITMAP *img2;
	ALLEGRO_BITMAP *img3;

	// position
	int x = -GAME_WIDTH, y = -GAME_HEIGHT;

};
Parallax new_parallax(ALLEGRO_BITMAP *i1, ALLEGRO_BITMAP *i2, ALLEGRO_BITMAP *i3);

// map struct
struct Map {

	// data
	int w = 0, h = 0;
	int grid[MAX_ARR_SIZE];
	Tileset tileset;
	Parallax parallax;

	// wind
	int windX = 2, windY = 1;

};
Map new_map(FILE *d, Tileset t, Parallax p);


// prototypes
// system
bool start();
bool init();
void exit();
void load_map_1();
void load_map_2();
void load_map_3();
void frame_update();
void frame_draw();
void game_restart();

// developer
void developper_show();
void developper_cmd(char cmd[]);
float get_frame_rate();

// create objects
void create_weapons();
void create_tilesets();

// character
void player_move(Character &p);
void enemy_move(Character &e);
void character_update_hit_buffer(Character &p);
void player_use_potion(Character &p);
void player_attack(Character &p);
void enemy_attack(Character &e);
void player_damage();
void enemy_damage();
void character_take_damage(Character &t, Character a);
void enemy_remove(Character &e);

void character_animate(Character &p);
void character_draw(Character p);
void character_draw_hp_bar(Character p);
void character_set_popup(Character &p);
void character_draw_popup(Character p);
void character_weapon_animate(Character &p);
void character_weapon_draw(Character p);
void character_weapon_range_draw(Character p);
void character_equip_weapon(Character &p, Weapon w);
bool character_in_front_of(Character t, Character s);

// item
void item_draw(Item i);
void item_equip(Character &p, Item &i);

// fountain
void fountain_draw(Fountain f);
void fountain_reward(Fountain &f);

// map
int map_at(Map m, int x, int y);
void map_draw(Map m);
void map_draw_parallax(Map &m, float opacity);

// collision
bool covering_tile(Character c, Map m);
bool covering_character(Character c);
bool covering_fountain(Character c);
bool out_of_map(Character c, Map m);

// hud
void hud_title_select(Hud &h);
void hud_index_scroll(Hud &h, int scene);
void hud_credits_scroll(Hud &h);

void hud_menu_toggle(Hud &h);
void hud_menu_select(Hud &h);
void hud_change_menu_music(Hud h);
void hud_update_frame_rate(Hud &h);

void hud_draw_title(Hud h);
void hud_draw(Hud &h, Character p);
void hud_draw_popup(Hud &h);
void hud_draw_menu(Hud h);

// event log
void event_log_add(Hud &h, char str[], int r, int g, int b);
void event_log_erase(Hud &h);

// camera
void camera_update(Camera &c, Character p);
void camera_move(ALLEGRO_TRANSFORM t, Camera c);
void camera_reset(ALLEGRO_TRANSFORM t);
bool in_camera_range(int x, int y, Camera c);

// tileset
void tileset_animate(Tileset &t);

// boss fight
void boss_fight_start();


// system variables
ALLEGRO_DISPLAY         *display = NULL;
ALLEGRO_EVENT_QUEUE     *ev_queue = NULL;
ALLEGRO_TIMER           *timer = NULL;
ALLEGRO_KEYBOARD_STATE  keyState;
ALLEGRO_TRANSFORM       transform;

// loading screen
ALLEGRO_BITMAP *loading_screen;

// draw flag
bool draw = true;

// objects
Character  player;
Map        map;
Hud        hud;
Camera     camera;
Tileset    tilesets[TILESET_NUM];
Weapon     weapons[WEAPON_SIZE];
Character  enemyList[MAX_ARR_SIZE];
Item       itemList[MAX_ARR_SIZE];
Fountain   fountainList[MAX_ARR_SIZE];

// sound variables
ALLEGRO_SAMPLE     *bgmNormal;
ALLEGRO_SAMPLE     *bgmBoss;
ALLEGRO_SAMPLE     *bgmTitle;
ALLEGRO_SAMPLE     *bgmWin;
ALLEGRO_SAMPLE     *bgmLose;
ALLEGRO_SAMPLE_ID  bgmNormalId;
ALLEGRO_SAMPLE_ID  bgmBossId;
ALLEGRO_SAMPLE_ID  bgmTitleId;
ALLEGRO_SAMPLE_ID  bgmWinId;
ALLEGRO_SAMPLE_ID  bgmLoseId;

// object sizes
int enemySize = ENEMY_NUM_1;
int itemSize = ITEM_NUM_1;
int fountainSize = FOUNTAIN_NUM_1;

// boss fight variables
bool fightingBoss = false;
bool bossKilled = false;

// level variables
bool quests[] = { false, false };
int level;

// scene
int scene = TITLE;

// restart / exit flag
bool restartGame = false;
bool exitGame = false;

// frame rate variables
clock_t oldClock;
clock_t newClock;
float framerate;

// developper variables
bool developperMode = false;
bool walkThroughWalls = DEXTAR;


int main() {

	// initialize game
	if (!start())
		return -1;

	while (true) {

		// create event and wait for an event to happen
		ALLEGRO_EVENT ev;
		al_wait_for_event(ev_queue, &ev);

		// if player closes game, exit loop
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || exitGame) {
			break;
		}

		// show developper console (if appropriate)
		developper_show();

		// update every frame
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			frame_update();
			draw = true;
		}

		// draw every frame
		if (draw) {
			draw = false;
			frame_draw();
		}

	}

	// exit game
	exit();

	return 0;
}


bool start() {

	// create random seed
	srand(time(0));

	// initialize addons and system
	if (!init())
		return false;

	// draw loading screen
	loading_screen = al_load_bitmap("Image/Hud/Text/text_loading.png");
	al_draw_bitmap(loading_screen, 0, 0, NULL);
	al_flip_display();

	// create weapons, tilesets
	create_weapons();
	create_tilesets();

	// reserve music samples
	al_reserve_samples(ENEMY_NUM_1 * 3 + ITEM_NUM_1 * 1 * FOUNTAIN_NUM_1 * 1 + 5 + 3);

	// create hud, camera
	hud = Hud();
	camera = Camera();

	// reset objects
	load_map_1();
	//load_map_2();

	// load and play music
	bgmNormal = al_load_sample("Audio/Bgm/bgm_normal.ogg");
	bgmBoss = al_load_sample("Audio/Bgm/bgm_boss.ogg");
	bgmTitle = al_load_sample("Audio/Bgm/bgm_title.ogg");
	bgmWin = al_load_sample("Audio/Bgm/bgm_win.ogg");
	bgmLose = al_load_sample("Audio/Bgm/bgm_lose.ogg");
	al_play_sample(bgmTitle, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmTitleId);

	// clear screen
	system("cls");

	// start timer
	al_start_timer(timer);
	oldClock = clock();

	return true;

}

bool init() {

	// if Allegro failed to initialize, return false
	if (!al_init()) {
		printf("Failed to initialize allegro\n"); // error message
		return false;
	}
	// if primitives addon not added, return false 
	if (!al_init_primitives_addon()) {
		printf("Failed to add primitives addon\n");
		return false;
	}
	// image 
	if (!al_init_image_addon()) {
		printf("Failed to add image addon\n");
		return false;
	}
	// keyboard
	if (!al_install_keyboard()) {
		printf("Failed to install keyboard\n");
		return false;
	}
	// mouse
	if (!al_install_mouse()) {
		printf("Failed to install mouse\n");
		return false;
	}
	// font
	if (!al_init_font_addon()) {
		printf("Failed to add font addon\n");
		return false;
	}
	// ttf
	if (!al_init_ttf_addon()) {
		printf("Failed to add ttf addon\n");
		return false;
	}
	// audio
	if (!al_install_audio()) {
		printf("failed to initialize audio\n");
		return false;
	}
	// codec
	if (!al_init_acodec_addon()) {
		printf("failed to initialize audio codecs\n");
		return false;
	}

	// fullscreen (if necessary)
	if (FULLSCREEN)
		al_set_new_display_flags(ALLEGRO_FULLSCREEN);

	// create display and set properties
	display = al_create_display(GAME_WIDTH, GAME_HEIGHT);
	al_set_window_title(display, WINDOW_TITLE);
	al_set_display_icon(display, WINDOW_ICON);

	// hide cursor (if necessary)
	if (HIDE_MOUSE)
		al_hide_mouse_cursor(display);

	// create evetn cure and timer
	ev_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	// if display not created, return false
	if (!display) {
		printf("Failed to create display\n"); // error message
		return false;
	}
	// if event queue not created, return false
	if (!ev_queue) {
		printf("Failed to create event queue\n"); // error message
		return false;
	}
	// if timer not created, return false
	if (!timer) {
		printf("Failed to create timer\n"); // error message
		return false;
	}

	// register event queue, timer
	al_register_event_source(ev_queue, al_get_keyboard_event_source());
	al_register_event_source(ev_queue, al_get_timer_event_source(timer));

	// set background to black and draw to display
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	return true;
}

void exit() {
	// destroy events
	al_destroy_display(display);
	al_destroy_event_queue(ev_queue);
	al_destroy_timer(timer);
}

void load_map_1() {

	// set level to 1
	level = 1;

	// reset event log
	event_log_erase(hud);

	// create map
	map = new_map(fopen("Data/map-001.txt", "r"), tilesets[0], new_parallax(NULL, NULL, al_load_bitmap("Image/Parallax/parallax_sun.png")));

	// create player
	if (DEXTAR)
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/dextar.png"), 32 * 19, 32 * 4, DOWN, 9999, 9999, 999, weapons[STICK], 0, 0);
	else
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/danniponcho.png"), 32 * 19, 32 * 4, DOWN, 200, 100, 10, weapons[STICK], 0, 0);

	// create enemies
	enemySize = ENEMY_NUM_1;
	enemyList[0] = new_character(ENEMY, C_DUMMY, al_load_bitmap("Image/Character/target.png"), 32 * 10.5, 32 * 45.5, DOWN, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[1] = new_character(ENEMY, C_DUMMY, al_load_bitmap("Image/Character/target.png"), 32 * 11, 32 * 43, DOWN, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[2] = new_character(ENEMY, C_DUMMY, al_load_bitmap("Image/Character/target.png"), 32 * 8, 32 * 46, DOWN, 50, 0, 0, weapons[STICK], 0, 999);

	// create items
	itemSize = ITEM_NUM_1;

	// create fountains
	fountainSize = FOUNTAIN_NUM_1;

	// clear screen
	system("cls");
}

void load_map_2() {

	// set level to 2
	level = 2;

	// reset event log
	event_log_erase(hud);

	// create map
	map = new_map(fopen("Data/map-002.txt", "r"), tilesets[0], new_parallax(al_load_bitmap("Image/Parallax/parallax_fog1.png"), al_load_bitmap("Image/Parallax/parallax_fog2.png"), al_load_bitmap("Image/Parallax/parallax_sun.png")));

	// create player
	if (DEXTAR)
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/dextar.png"), 32 * 3, 32 * 4, DOWN, 9999, 9999, 999, weapons[STICK], 0, 0);
	else
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/danniponcho.png"), 32 * 3, 32 * 4, DOWN, 200, 100, 10, weapons[STICK], 0, 0);

	// create enemies
	enemySize = ENEMY_NUM_2;
	enemyList[0] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 8, 32 * 16, DOWN, 80, 0, 12, weapons[STICK], 0, 999);
	enemyList[1] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 17, 32 * 45, DOWN, 80, 0, 12, weapons[STICK], 0, 999);
	enemyList[2] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 3, 32 * 31, DOWN, 80, 0, 12, weapons[STICK], 0, 999);
	enemyList[3] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 22, 32 * 18, DOWN, 80, 0, 12, weapons[STICK], 0, 999);

	enemyList[4] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 6, 32 * 11, DOWN, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[5] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 7.5, 32 * 10, LEFT, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[6] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 8, 32 * 9, RIGHT, 50, 0, 0, weapons[STICK], 0, 999);

	enemyList[7] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 12, 32 * 23, RIGHT, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[8] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 13, 32 * 23.5, LEFT, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[9] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/rocks.png"), 32 * 14, 32 * 24.5, DOWN, 50, 0, 0, weapons[STICK], 0, 999);

	// create items
	itemSize = ITEM_NUM_2;

	// create fountains
	fountainSize = FOUNTAIN_NUM_2;
	fountainList[0] = new_fountain(al_load_bitmap("Image/Object/fountain_active.png"), al_load_bitmap("Image/Object/fountain_inactive.png"), 20, 32 * 20, 32 * 47, 0, 1);

	// clear screen
	system("cls");
}

void load_map_3() {

	// set level to 3
	level = 3;

	// reset event log
	event_log_erase(hud);

	// create map
	map = new_map(fopen("Data/map-003.txt", "r"), tilesets[0], new_parallax(al_load_bitmap("Image/Parallax/parallax_fog1.png"), al_load_bitmap("Image/Parallax/parallax_fog2.png"), al_load_bitmap("Image/Parallax/parallax_sun.png")));

	// create player
	if (DEXTAR)
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/dextar.png"), 32 * 6, 32 * 4, DOWN, 9999, 9999, 999, weapons[STICK], 0, 0);
	else
		player = new_character(PLAYER, C_PLAYER, al_load_bitmap("Image/Character/danniponcho.png"), 32 * 6, 32 * 4, DOWN, 200, 100, 10, weapons[STICK], 0, 0);

	// create enemies
	enemySize = ENEMY_NUM_3;
	enemyList[0] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 9, 32 * 11, DOWN, 80, 0, 12, weapons[STICK], 0, 30);
	enemyList[1] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 18, 32 * 16, DOWN, 80, 0, 12, weapons[STICK], 0, 30);
	enemyList[2] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 14, 32 * 26, DOWN, 80, 0, 12, weapons[STICK], 0, 30);
	enemyList[3] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 9, 32 * 38, DOWN, 80, 0, 12, weapons[STICK], 37, 61);
	enemyList[4] = new_character(ENEMY, C_PIRATE, al_load_bitmap("Image/Character/pirate.png"), 32 * 10, 32 * 38, DOWN, 100, 0, 15, weapons[SWORD], 37, 61);
	enemyList[4].itemDrop = POTION;
	enemyList[5] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 12, 32 * 52, DOWN, 80, 0, 12, weapons[STICK], 37, 61);
	enemyList[6] = new_character(ENEMY, C_TELEMARKETER, al_load_bitmap("Image/Character/telemarketer.png"), 32 * 17, 32 * 55, DOWN, 80, 0, 12, weapons[STICK], 37, 61);
	enemyList[7] = new_character(ENEMY, C_PIRATE, al_load_bitmap("Image/Character/pirate.png"), 32 * 10, 32 * 59, DOWN, 100, 0, 15, weapons[SWORD], 37, 61);
	enemyList[7].itemDrop = POTION;

	enemyList[8] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/barrels.png"), 32 * 10.5, 30 * 34, DOWN, 50, 0, 0, weapons[STICK], 0, 999);
	enemyList[9] = new_character(ENEMY, C_OBJECT, al_load_bitmap("Image/Character/barrels.png"), 32 * 11, 32 * 34, DOWN, 50, 0, 0, weapons[STICK], 0, 999);

	// boss
	enemyList[10] = new_character(BOSS, C_YIRAN, al_load_bitmap("Image/Character/yiran.png"), 32 * 10, 32 * 69, DOWN, 500, 0, 35, weapons[CELLO], 65, 999);
	enemyList[10].stepSize = 4;
	enemyList[10].followRange = 8;

	// create items
	itemSize = ITEM_NUM_3;
	itemList[0] = new_item(ITEM, POTION, al_load_bitmap("Image/Object/potion.png"), 32 * 12, 32 * 45);
	itemList[1] = new_item(WEAPON, SWORD, al_load_bitmap("Image/Object/sword.png"), 32 * 10, 32 * 70);

	// create fountains
	fountainSize = FOUNTAIN_NUM_3;
	fountainList[0] = new_fountain(al_load_bitmap("Image/Object/fountain_active.png"), al_load_bitmap("Image/Object/fountain_inactive.png"), 20, 32 * 17, 32 * 46, 0.5, 0.5);

	// clear screen
	system("cls");
}

void frame_update() {

	// restart the game (if appropriate)
	game_restart();
	// update the frame rate
	hud_update_frame_rate(hud);

	// toggle menu
	hud_menu_toggle(hud);

	if (scene == GAME) {

		// if quest completed -> next level
		if (level == 1) {
			if (quests[0] == true) {
				Inventory i = player.inventory;
				load_map_2();
				player.inventory = i;
			}
		}
		if (level == 2) {
			if (quests[1] == true) {
				Inventory i = player.inventory;
				load_map_3();
				player.inventory = i;
			}
		}
		if (level == 3) {
			if (player.y >= 64 * 32) {
				boss_fight_start();
			}
		}

		// enemies
		for (int i = 0; i < enemySize; i++) {
			// update hit buffer
			character_update_hit_buffer(enemyList[i]);
			if (in_camera_range(enemyList[i].x, enemyList[i].y, camera)) {
				// move and attack
				enemy_move(enemyList[i]);
				enemy_attack(enemyList[i]);
				// animate
				character_animate(enemyList[i]);
				character_weapon_animate(enemyList[i]);
			}

		}

		// player
		// update hit buffer
		character_update_hit_buffer(player);
		character_set_popup(player);
		// move, attack and use potion
		player_move(player);
		player_attack(player);
		player_use_potion(player);
		// animate
		character_animate(player);
		character_weapon_animate(player);

		// pick up weapons
		for (int i = 0; i < itemSize; i++) {
			if (in_camera_range(itemList[i].x, itemList[i].y, camera))
				item_equip(player, itemList[i]);
		}

		// reward from fountains
		for (int i = 0; i < fountainSize; i++) {
			//if (in_camera_range(fountainList[i].x, fountainList[i].y, camera)) {
			fountain_reward(fountainList[i]);
			//}
		}

		// take damage
		enemy_damage();
		player_damage();

		// update camera
		camera_update(camera, player);

	}

	if (scene == MENU) {
		// update menu
		hud_index_scroll(hud, scene);
		hud_menu_select(hud);
	}
	if (scene == MENU_CONTROLS) {
		// update menu
		hud_menu_select(hud);
	}

	if (scene == TITLE) {
		// update title screen
		hud_index_scroll(hud, scene);
		hud_title_select(hud);
	}

	if (scene == CREDITS) {
		// update title screen
		hud_credits_scroll(hud);
		hud_title_select(hud);
	}

	if (scene == INSTRUCTIONS) {
		// update title screen
		hud_title_select(hud);
	}

}

void frame_draw() {

	// set background to black
	al_clear_to_color(al_map_rgb(0, 0, 0));

	if (scene == GAME || scene == MENU || scene == MENU_CONTROLS) {

		// move camera
		camera_move(transform, camera);

		// draw
		tileset_animate(map.tileset);
		map_draw(map);

		// draw fountains
		for (int i = 0; i < fountainSize; i++) {
			if (in_camera_range(fountainList[i].x, fountainList[i].y, camera)) {
				fountain_draw(fountainList[i]);
			}
		}

		// draw range attacks
		for (int i = 0; i < enemySize; i++) {
			if (in_camera_range(enemyList[i].x, enemyList[i].y, camera)) {
				character_weapon_range_draw(enemyList[i]);
			}
		}
		character_weapon_range_draw(player);

		// draw player
		if (player.dir == LEFT || player.dir == UP) // left handed - draw in back if not facing right
			character_weapon_draw(player);
		character_draw(player);
		if (player.dir == RIGHT || player.dir == DOWN) // left handed - draw in front if facing right
			character_weapon_draw(player);

		// draw enemies
		for (int i = 0; i < enemySize; i++) {
			if (in_camera_range(enemyList[i].x, enemyList[i].y, camera)) {
				if (enemyList[i].dir == RIGHT || enemyList[i].dir == UP) // right handed - draw in back if not facing left
					character_weapon_draw(enemyList[i]);
				character_draw(enemyList[i]);
				if (enemyList[i].dir == LEFT || enemyList[i].dir == DOWN) // right handed - draw in front if facing left
					character_weapon_draw(enemyList[i]);
			}
		}

		// draw items
		for (int i = 0; i < itemSize; i++) {
			if (in_camera_range(itemList[i].x, itemList[i].y, camera))
				item_draw(itemList[i]);
		}

		// draw hp bars
		for (int i = 0; i < enemySize; i++) {
			if (in_camera_range(enemyList[i].x, enemyList[i].y, camera))
				character_draw_hp_bar(enemyList[i]);
		}
		character_draw_hp_bar(player);

		// draw player popup
		character_draw_popup(player);

		// reset camera
		camera_reset(transform);

		// draw hud and parallax
		map_draw_parallax(map, PARALLAX_OPACITY);
		hud_draw(hud, player);
		// draw popups (if necessary)
		if (scene == GAME)
			hud_draw_popup(hud);
	}

	if (scene == MENU || scene == MENU_CONTROLS) {
		// draw menu
		hud_draw_menu(hud);
	}

	if (scene == TITLE) {
		// draw title
		hud_draw_title(hud);
	}

	if (scene == CREDITS) {
		// draw credits
		al_draw_bitmap(hud.title_back, 0, 0, NULL);
		al_draw_bitmap(hud.credits, 0, hud.creditsY, NULL);

		if (hud.creditsY > -(al_get_bitmap_height(hud.credits) - GAME_HEIGHT))
			al_draw_bitmap(hud.arrow_down, 0, 0, NULL);
		if (hud.creditsY < 0)
			al_draw_bitmap(hud.arrow_up, 0, 0, NULL);
	}

	if (scene == INSTRUCTIONS) {
		// draw instructions
		al_draw_bitmap(hud.title_back, 0, 0, NULL);
		al_draw_bitmap(hud.instructions, 0, 0, NULL);
	}

	//draw to display
	al_flip_display();

}

void game_restart() {

	// if restart form menu
	if (restartGame) {
		restartGame = false;

		load_map_1();

		for (int i = 0; i < 2; i++) {
			quests[i] = false;
		}

		if (fightingBoss) {
			fightingBoss = false;
			bossKilled = false;
			hud.bossFightPopup = false;
		}
		al_stop_sample(&bgmNormalId);
		al_stop_sample(&bgmBossId);
		al_stop_sample(&bgmTitleId);
		al_stop_sample(&bgmWinId);
		al_stop_sample(&bgmLoseId);
		developperMode = false;
		walkThroughWalls = DEXTAR;
		if (hud.music)
			al_play_sample(bgmNormal, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmNormalId);
		scene = GAME;

		return;
	}

	if (!player.dead && !bossKilled) {
		return;
	}

	if (scene == MENU)
		return;

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (al_key_down(&keyState, KEY_SELECT)) {

		load_map_1();

		for (int i = 0; i < 2; i++) {
			quests[i] = false;
		}

		if (fightingBoss) {
			fightingBoss = false;
			bossKilled = false;
			hud.bossFightPopup = false;
		}
		al_stop_sample(&bgmNormalId);
		al_stop_sample(&bgmBossId);
		al_stop_sample(&bgmTitleId);
		al_stop_sample(&bgmWinId);
		al_stop_sample(&bgmLoseId);
		developperMode = false;
		walkThroughWalls = DEXTAR;
		if (hud.sound)
			al_play_sample(hud.se_select, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		if (hud.music)
			al_play_sample(bgmNormal, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmNormalId);
		scene = GAME;
	}
}


void developper_show() {

	// return if console not enabled
	if (!CONSOLE_ENABLED)
		return;

	// get keyboard state
	al_get_keyboard_state(&keyState);
	if (al_key_down(&keyState, ALLEGRO_KEY_SEMICOLON) && al_key_down(&keyState, ALLEGRO_KEY_BACKSLASH)) {

		// draw command prompt to display
		al_draw_bitmap(hud.text_console, 0, 0, NULL);
		al_flip_display();

		// stop timer (game stops)
		al_stop_timer(timer);
		system("cls");
		printf("Console commands:\n");
		printf("1. developper_on:       \t turns developper mode on\n");
		printf("2. developper_off:      \t turns developper mode off\n");
		printf("3. walkThroughWalls_on: \t turns walk through walls mode on\n");
		printf("4. walkThroughWalls_off:\t turns walk through walls off\n");
		char cmd[40];
		scanf("%s", cmd);
		developper_cmd(cmd);
		// start timer (game start)
		al_start_timer(timer);
	}
}

void developper_cmd(char cmd[]) {

	// turn developper mode on
	if (strcmp(cmd, "developper_on") == 0 || strcmp(cmd, "1") == 0) {
		developperMode = true;
		player.inventory.potions = 999;
		player.inventory.gold = 1000;
		player.maxHp = 9999;
		player.hp = 9999;
		player.mpRegen = player.maxMp;
	}
	// turn developper mode off
	if (strcmp(cmd, "developper_off") == 0 || strcmp(cmd, "2") == 0) {
		developperMode = false;
	}
	// turn walk through walls on
	if (strcmp(cmd, "walkThroughWalls_on") == 0 || strcmp(cmd, "3") == 0) {
		walkThroughWalls = true;
	}
	// turn walk through walls off
	if (strcmp(cmd, "walkThroughWalls_off") == 0 || strcmp(cmd, "4") == 0) {
		walkThroughWalls = false;
	}
}

float get_frame_rate() {

	// use clocks to get time between frames
	newClock = clock();
	// get millis passed
	float diff = ((float)(newClock - oldClock) / CLOCKS_PER_SEC) * 1000;
	oldClock = newClock;

	// return framerate
	return 1000 / diff;
}


Character new_character(int t, int cT, ALLEGRO_BITMAP *s, int x, int y, int dir, int h, int m, int a, Weapon w, int minY, int maxY) {
	// create character
	Character p = Character();
	// set parameters
	p.type = t;
	p.characterType = cT;
	p.spritesheet = s;
	p.x = x;
	p.y = y;
	p.dir = dir;
	p.hp = h;
	p.maxHp = h;
	p.mp = m;
	p.maxMp = m;
	p.atk = a;
	p.weapon = w;
	p.yMin = minY;
	p.yMax = maxY;
	//return p
	return p;
}

Map new_map(FILE *d, Tileset t, Parallax p) {
	// create map
	Map m = Map();
	// set parameters
	m.tileset = t;
	fscanf(d, "%d", &m.w);
	fscanf(d, "%d", &m.h);
	for (int i = 0; i < m.h; i++) {
		for (int j = 0; j < m.w; j++) {
			fscanf(d, "%d", &m.grid[i * m.w + j]);
		}
	}
	m.parallax = p;
	// return m
	return m;
}

Weapon new_weapon(int t, int wN, ALLEGRO_BITMAP *s, ALLEGRO_BITMAP *r, int d, ALLEGRO_SAMPLE *se[]) {
	// create weapon
	Weapon w = Weapon();
	// set parameters
	w.type = t;
	w.name = wN;
	w.spritesheet = s;
	w.rangePic = r;
	w.damage = d;
	for (int i = 0; i < WEAPON_SOUND_NUM; i++)
		w.se[i] = se[i];
	// return w
	return w;
}

Item new_item(int t, int id, ALLEGRO_BITMAP *img, int x, int y) {
	Item i = Item();
	i.type = t;
	i.id = id;
	i.image = img;
	i.x = x;
	i.y = y;
	return i;
}

Fountain new_fountain(ALLEGRO_BITMAP *iA, ALLEGRO_BITMAP *iI, int c, int x, int y, float wC, float iC) {
	Fountain f = Fountain();
	f.image_active = iA;
	f.image_inactive = iI;
	f.cost = c;
	f.x = x;
	f.y = y;
	f.weaponChance = wC;
	f.itemChance = iC;
	return f;
}

Tileset new_tileset(ALLEGRO_BITMAP *i, FILE *d) {
	// create tileset
	Tileset t = Tileset();
	// set parameters
	t.image = i;

	fscanf(d, "%d", &t.h);
	for (int i = 0; i < t.h; i++) {
		fscanf(d, "%d", &t.data[i]);
	}
	// return t
	return t;
}

Parallax new_parallax(ALLEGRO_BITMAP *i1, ALLEGRO_BITMAP *i2, ALLEGRO_BITMAP *i3) {
	Parallax p = Parallax();
	p.img1 = i1;
	p.img2 = i2;
	p.img3 = i3;
	return p;
}


void create_weapons() {
	// create stick, sword and cello
	ALLEGRO_SAMPLE *se1[] = { al_load_sample("audio/Se/se_blow.ogg"), al_load_sample("audio/Se/se_blow.ogg"), al_load_sample("audio/Se/se_blow.ogg") };
	weapons[STICK] = new_weapon(WEAPON_MELEE, STICK, al_load_bitmap("Image/Weapon/stick.png"), NULL, 0, se1);
	ALLEGRO_SAMPLE *se2[] = { al_load_sample("audio/Se/se_sword.ogg"), al_load_sample("audio/Se/se_sword.ogg"), al_load_sample("audio/Se/se_sword.ogg") };
	weapons[SWORD] = new_weapon(WEAPON_MELEE, SWORD, al_load_bitmap("Image/Weapon/sword.png"), NULL, 10, se2);
	ALLEGRO_SAMPLE *se3[] = { al_load_sample("audio/Se/se_cello_1.ogg"), al_load_sample("audio/Se/se_cello_3.ogg"), al_load_sample("audio/Se/se_cello_2.ogg") };
	weapons[CELLO] = new_weapon(WEAPON_RANGE, CELLO, al_load_bitmap("Image/Weapon/cello.png"), al_load_bitmap("Image/Weapon/cello_range.png"), 0, se3);
}

void create_tilesets() {
	// create grass tileset
	tilesets[0] = new_tileset(al_load_bitmap("Image/Tileset/tileset-001.png"), fopen("Data/tileset-001.txt", "r"));
}


void player_move(Character &p) {

	// return if player is dead
	if (player.dead)
		return;

	p.moving = true;

	// get keyboard state
	al_get_keyboard_state(&keyState);

	// set dashing speed and state
	if (al_key_down(&keyState, KEY_DASH)) {
		p.dashing = true;
	}
	else {
		p.dashing = false;
	}
	int stepSize = p.stepSize * ((p.dashing) ? (float)p.dashMultiplier : 1.0);

	// if down pressed, set direction to DOWN and move down
	if (al_key_down(&keyState, KEY_DOWN)) {
		p.dir = DOWN;
		p.y += stepSize;
		while ((covering_tile(player, map) || out_of_map(player, map) || covering_character(player) || covering_fountain(player)) && !walkThroughWalls)
			p.y -= 1;
	}
	// if up pressed, set direction to UP and move up
	else if (al_key_down(&keyState, KEY_UP)) {
		p.dir = UP;
		p.y -= stepSize;
		while ((covering_tile(player, map) || out_of_map(player, map) || covering_character(player) || covering_fountain(player)) && !walkThroughWalls)
			p.y += 1;
	}
	// if right pressed, set direction to RIGHT and move right	
	else if (al_key_down(&keyState, KEY_RIGHT)) {
		p.dir = RIGHT;
		p.x += stepSize;
		while ((covering_tile(player, map) || out_of_map(player, map) || covering_character(player) || covering_fountain(player)) && !walkThroughWalls)
			p.x -= 1;
	}
	// if left pressed, set direction to LEFT and move left	
	else if (al_key_down(&keyState, KEY_LEFT)) {
		p.dir = LEFT;
		p.x -= stepSize;
		while ((covering_tile(player, map) || out_of_map(player, map) || covering_character(player) || covering_fountain(player)) && !walkThroughWalls)
			p.x += 1;
	}
	else {
		p.moving = false;
	}
}

void enemy_move(Character &e) {

	if (e.characterType == C_DUMMY || e.characterType == C_OBJECT)
		return;

	float xDiff = abs(player.x - e.x);
	float yDiff = abs(player.y - e.y);
	bool inRange = !(sqrt(xDiff * xDiff + yDiff * yDiff) >= e.followRange * 32);

	if (player.dead)
		inRange = false;

	if (e.attacking && e.weapon.type == WEAPON_RANGE)
		return;

	int stepSize = (inRange) ? e.stepSize : e.stepSize / 2;

	bool moved = false;

	if (!inRange) {
		int changeDir = rand() % 16;
		if (changeDir == 0) {
			int turnDir = rand() % 2;
			e.dir += turnDir;
			e.dir %= 4;
		}

		if (e.dir == UP) {
			e.y -= stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y < e.yMin * 32) {
				e.y += 1;
				moved = false;
			}
		}
		if (e.dir == DOWN) {
			e.y += stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y > e.yMax * 32) {
				e.y -= 1;
				moved = false;
			}
		}
		if (e.dir == LEFT) {
			e.x -= stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
				e.x += 1;
				moved = false;
			}
		}
		if (e.dir == RIGHT) {
			e.x += stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
				e.x -= 1;
				moved = false;
			}
		}

		return;
	}

	if (xDiff > yDiff) {
		if (player.x - e.x > 16 && !moved) {
			e.dir = RIGHT;
			e.x += stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
				e.x -= 1;
				moved = false;
			}
		}
		if (player.x - e.x < -16 && !moved) {
			e.dir = LEFT;
			e.x -= stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
				e.x += 1;
				moved = false;
			}
		}
	}

	if (xDiff < yDiff) {
		if (player.y - e.y > 16 && !moved) {
			e.dir = DOWN;
			e.y += stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y > e.yMax * 32) {
				e.y -= 1;
				moved = false;
			}
		}
		if (player.y - e.y < -16 && !moved) {
			e.dir = UP;
			e.y -= stepSize;
			moved = true;
			while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y < e.yMin * 32) {
				e.y += 1;
				moved = false;
			}
		}
	}

	if (player.x - e.x > 16 && !moved) {
		e.dir = RIGHT;
		e.x += stepSize;
		moved = true;
		while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
			e.x -= 1;
			moved = false;
		}
	}
	if (player.x - e.x < -16 && !moved) {
		e.dir = LEFT;
		e.x -= stepSize;
		moved = true;
		while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e)) {
			e.x += 1;
			moved = false;
		}
	}
	if (player.y - e.y > 16 && !moved) {
		e.dir = DOWN;
		e.y += stepSize;
		moved = true;
		while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y > e.yMax * 32) {
			e.y -= 1;
			moved = false;
		}
	}
	if (player.y - e.y < -16 && !moved) {
		e.dir = UP;
		e.y -= stepSize;
		moved = true;
		while (covering_tile(e, map) || out_of_map(e, map) || covering_character(e) || covering_fountain(e) || e.y < e.yMin * 32) {
			e.y += 1;
			moved = false;
		}
	}
}

void character_update_hit_buffer(Character &p) {
	// return if player is not invincible
	if (!p.invincible) return;
	// update hit buffer
	p.hitBuffCheck++;
	// if time to remove invincibility, remove invincibility
	if (p.hitBuffCheck >= p.hitBuff) {
		p.invincible = false;
		p.hitBuffCheck = 0;
	}
}

void player_use_potion(Character &p) {

	// return if player is dead
	if (player.dead)
		return;

	p.inventory.potionFreqCheck++;

	if (p.inventory.potionFreqCheck < p.inventory.potionFreq)
		return;

	// get keyboard state
	al_get_keyboard_state(&keyState);

	// if space pressed, set attacking to true
	if (al_key_down(&keyState, KEY_ITEM)) {
		if (p.inventory.potions > 0) {

			quests[1] = true;

			// add potions
			p.inventory.potions--;
			p.hp += 100;
			if (p.hp > p.maxHp)
				p.hp = p.maxHp;

			// display on event log
			event_log_add(hud, "Used 1 potion", 128, 255, 128);

			// play sound
			if (hud.sound)
				al_play_sample(p.se_heal, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			p.inventory.potionFreqCheck = 0;
		}
	}
}

void player_attack(Character &p) {

	// return if player is dead
	if (player.dead)
		return;

	// regen energy
	p.mp += p.mpRegen;
	if (p.mp > p.maxMp)
		p.mp = p.maxMp;

	if (p.attacking)
		return;
	if (p.mp < 10)
		return;

	// get keyboard state
	al_get_keyboard_state(&keyState);

	// if space pressed, set attacking to true
	if (al_key_down(&keyState, KEY_ATTACK)) {
		p.attacking = true;
		p.mp -= 10;
		// play sound
		if (hud.sound)
			al_play_sample(p.weapon.se[p.weapon.seId], SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		p.weapon.seId++;
		p.weapon.seId %= WEAPON_SOUND_NUM;
	}
}

void enemy_attack(Character &e) {

	if (e.characterType == C_DUMMY || e.characterType == C_OBJECT)
		return;

	if (player.dead)
		return;

	if (e.weapon.type == WEAPON_MELEE) {
		if (!character_in_front_of(player, e)) {
			e.atkFreqCheck = 999;
			return;
		}
	}

	if (e.weapon.type == WEAPON_RANGE) {
		float xDiff = abs(player.x - e.x);
		float yDiff = abs(player.y - e.y);
		if (sqrt(xDiff * xDiff + yDiff * yDiff) > e.weapon.rangeDist * 32) {
			e.atkFreqCheck = 999;
			return;
		}
	}

	e.atkFreqCheck++;
	if (e.atkFreqCheck >= e.atkFreq) {
		int attack = rand() % e.atkRand;
		if (attack == 0) {
			e.atkFreqCheck = 0;
			e.attacking = true;
			if (hud.sound)
				al_play_sample(e.weapon.se[e.weapon.seId], SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			e.weapon.seId++;
			e.weapon.seId %= WEAPON_SOUND_NUM;
		}
	}
}

void player_damage() {

	// return if player is dead
	if (player.dead)
		return;

	// loop through enemies and take damage if in range and attacking
	for (int i = 0; i < enemySize; i++) {
		if (enemyList[i].attacking) {
			if (enemyList[i].weapon.type == WEAPON_MELEE) {
				if (character_in_front_of(player, enemyList[i])) {
					if (!player.invincible) {
						character_take_damage(player, enemyList[i]);
						player.invincible = true;
						if (hud.sound)
							al_play_sample(player.se_damage, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
			}
			if (enemyList[i].weapon.type == WEAPON_RANGE) {
				float xDiff = abs(player.x - enemyList[i].x);
				float yDiff = abs(player.y - enemyList[i].y);
				if (sqrt(xDiff * xDiff + yDiff * yDiff) <= enemyList[i].weapon.rangeDist * 32) {
					if (!player.invincible) {
						character_take_damage(player, enemyList[i]);
						player.invincible = true;
						if (hud.sound)
							al_play_sample(player.se_damage, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
			}
		}
	}
}

void enemy_damage() {
	// return if player not attacking
	if (!player.attacking)
		return;

	// loop through enemies and attack player if attacking and in range
	for (int i = 0; i < enemySize; i++) {
		if (in_camera_range(enemyList[i].x, enemyList[i].y, camera)) {
			if (player.weapon.type == WEAPON_MELEE) {
				if (character_in_front_of(enemyList[i], player)) {
					if (!enemyList[i].invincible) {
						character_take_damage(enemyList[i], player);
						enemyList[i].invincible = true;
						if (hud.sound)
							al_play_sample(enemyList[i].se_damage, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
			}
			if (player.weapon.type == WEAPON_RANGE) {
				float xDiff = abs(player.x - enemyList[i].x);
				float yDiff = abs(player.y - enemyList[i].y);
				if (sqrt(xDiff * xDiff + yDiff * yDiff) <= enemyList[i].weapon.rangeDist * 32) {
					if (!enemyList[i].invincible) {
						character_take_damage(enemyList[i], player);
						enemyList[i].invincible = true;
						if (hud.sound)
							al_play_sample(enemyList[i].se_damage, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
			}
		}
	}
}

void character_take_damage(Character &t, Character a) {

	// remove hp
	t.hp -= a.atk + a.weapon.damage;
	if (t.hp <= 0) {

		// event log add
		if (t.characterType == C_PLAYER)
			event_log_add(hud, "Danniponcho defeated", 255, 128, 128);
		if (t.characterType == C_YIRAN)
			event_log_add(hud, "Yiran defeated, +100G", 255, 128, 128);
		if (t.characterType == C_TELEMARKETER)
			event_log_add(hud, "Telemarketer defeated, +10G", 255, 255, 255);
		if (t.characterType == C_PIRATE)
			event_log_add(hud, "Pirate defeated, +10G", 255, 255, 255);

		// set item drop
		if (t.itemDrop == POTION) {
			itemSize++;
			itemList[itemSize - 1] = new_item(ITEM, POTION, al_load_bitmap("Image/Object/potion.png"), t.x, t.y);
		}

		// end boss fight
		if (t.type == BOSS) {
			player.inventory.gold += 100;
			enemy_remove(t);
			bossKilled = true;
			if (hud.music) {
				al_stop_sample(&bgmNormalId);
				al_stop_sample(&bgmBossId);
				al_stop_sample(&bgmTitleId);
				al_stop_sample(&bgmWinId);
				al_stop_sample(&bgmLoseId);
				al_play_sample(bgmWin, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &bgmWinId);
			}
			return;
		}

		// drop items
		if (t.type == ENEMY) {

			if (t.characterType == C_OBJECT)
				player.inventory.gold += 2;
			else if (t.characterType == C_DUMMY) {}
			else
				player.inventory.gold += 10;

			enemy_remove(t);
			return;
		}

		// game over
		if (t.type == PLAYER) {
			player.hp = 0;
			player.dead = true;
			if (hud.music) {
				al_stop_sample(&bgmNormalId);
				al_stop_sample(&bgmBossId);
				al_stop_sample(&bgmTitleId);
				al_stop_sample(&bgmWinId);
				al_stop_sample(&bgmLoseId);
				al_play_sample(bgmLose, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &bgmLoseId);
			}
			return;
		}
	}

}

void enemy_remove(Character &e) {
	e = enemyList[enemySize - 1];
	enemySize--;

	if (enemySize == 0)
		quests[0] = true;

}

void character_animate(Character &p) {
	if (p.moving) {
		// update animation check
		p.animFreqCheck++;
		// if tiem to update frame X, update frame X
		if (p.animFreqCheck >= p.animFreq) {
			p.frameX++;
			p.frameX %= 3;
			p.animFreqCheck = 0;
		}
	}
	else {
		// set frame X to 1 (idle position)
		p.frameX = 1;
		p.animFreqCheck = 0;
	}
	// set frame Y to player direciton
	p.frameY = p.dir;
}

void character_draw(Character p) {
	if (p.dead)
		return;

	al_draw_bitmap_region(p.spritesheet, p.frameX * 32, p.frameY * 32, 32, 32, p.x, p.y, NULL);
}

void character_draw_hp_bar(Character p) {

	// return if character is dead
	if (p.dead)
		return;

	// draw boss hp bar
	if (p.type == BOSS) {
		al_draw_filled_rectangle(p.x - 15, p.y - 16, p.x + 47, p.y - 8, al_map_rgb(200, 0, 0));
		al_draw_filled_rectangle(p.x - 15, p.y - 16, p.x - 15 + 62 * ((float)p.hp / p.maxHp), p.y - 8, al_map_rgb(0, 200, 0));
		al_draw_rectangle(p.x - 15, p.y - 16, p.x + 47, p.y - 8, al_map_rgb(0, 0, 0), 1);
	}
	// draw regular hp bar
	else {
		al_draw_filled_rectangle(p.x + 1, p.y - 16, p.x + 31, p.y - 8, al_map_rgb(200, 0, 0));
		al_draw_filled_rectangle(p.x + 1, p.y - 16, p.x + 1 + 30 * ((float)p.hp / p.maxHp), p.y - 8, al_map_rgb(0, 200, 0));
		al_draw_rectangle(p.x + 1, p.y - 16, p.x + 31, p.y - 8, al_map_rgb(0, 0, 0), 1);
	}
}

void character_set_popup(Character &p) {

	p.popup = false;

	// set fountain popup if in range and active
	for (int i = 0; i < fountainSize; i++) {

		float xDiff = abs(player.x - fountainList[i].x);
		float yDiff = abs(player.y - fountainList[i].y);
		if (sqrt(xDiff * xDiff + yDiff * yDiff) <= 48) {

			if (fountainList[i].reloadFreqCheck >= fountainList[i].reloadFreq) {
				if (p.inventory.gold < fountainList[i].cost)
					p.balloonPopup = fountainList[i].balloonPopupInable;
				else
					p.balloonPopup = fountainList[i].balloonPopupAble;
				p.popup = true;
			}

		}
	}

	// set enemy popup if target
	for (int i = 0; i < enemySize; i++) {

		if (enemyList[i].characterType == C_DUMMY) {

			if (character_in_front_of(enemyList[i], p)) {
				p.balloonPopup = enemyList[i].enemyPopup;
				p.popup = true;
			}

		}

	}

	// set potion popup if never used
	if (quests[1] == false && p.inventory.potions > 0) {
		p.balloonPopup = p.inventory.potionPopup;
		p.popup = true;
	}

}

void character_draw_popup(Character p) {
	if (!p.popup)
		return;

	al_draw_bitmap(p.balloonPopup, p.x, p.y - 52, NULL);
}


void character_weapon_animate(Character &p) {
	// return if player is not attacking
	if (!p.attacking) {
		return;
	}
	// update animation check
	p.weapon.animFreqCheck++;
	if (p.weapon.animFreqCheck >= p.weapon.animFreq) { // if time to update frame X, update frame X
		p.weapon.frameX++;
		p.weapon.frameX %= 5;
		p.weapon.animFreqCheck = 0;
		// when animation is over, set atack to false
		if (p.weapon.frameX == 0) {
			p.attacking = false;
		}
	}
	// set frame Y to player direciton
	p.weapon.frameY = p.dir;
}

void character_weapon_draw(Character p) {
	if (p.dead)
		return;

	// return if player is not attacking
	if (!p.attacking) {
		return;
	}
	if (p.weapon.spritesheet == NULL) {
		return;
	}

	al_draw_bitmap_region(p.weapon.spritesheet, p.weapon.frameX * 64, p.weapon.frameY * 64, 64, 64, p.x - 16, p.y - 16, ALLEGRO_FLIP_HORIZONTAL);
}

void character_weapon_range_draw(Character p) {
	if (p.dead)
		return;

	if (!p.attacking) {
		return;
	}

	if (p.weapon.type == WEAPON_RANGE) {
		al_draw_bitmap(p.weapon.rangePic, p.x + 16 - al_get_bitmap_width(p.weapon.rangePic) / 2, p.y + 16 - al_get_bitmap_height(p.weapon.rangePic) / 2, NULL);
	}
}

void character_equip_weapon(Character &p, Weapon w) {
	p.weapon = w;
}

bool character_in_front_of(Character t, Character s) {
	switch (s.dir) {
	case DOWN:
		return abs(t.x - s.x) < 24 && t.y >= s.y && t.y - s.y < 40;
	case UP:
		return abs(t.x - s.x) < 24 && t.y <= s.y && t.y - s.y > -40;
	case RIGHT:
		return abs(t.y - s.y) < 24 && t.x >= s.x && t.x - s.x < 40;
	case LEFT:
		return abs(t.y - s.y) < 24 && t.x <= s.x && t.x - s.x > -40;
	}
	return false;
}


void item_draw(Item i) {
	al_draw_bitmap(i.image, i.x, i.y - (al_get_bitmap_height(i.image) - 32), NULL);
}

void item_equip(Character &p, Item &i) {

	float xDiff = abs(p.x - i.x);
	float yDiff = abs(p.y - i.y);
	if (sqrt(xDiff * xDiff + yDiff * yDiff) > 36)
		return;

	if (i.type == WEAPON) {
		p.weapon = weapons[i.id];
		player.attacking = false;

		if (i.id == SWORD)
			event_log_add(hud, "Sword equipped", 255, 255, 0);
		if (i.id == CELLO)
			event_log_add(hud, "Cello equipped", 255, 255, 0);
	}
	if (i.type == ITEM) {
		if (i.id == POTION) {
			p.inventory.potions += 1;
			event_log_add(hud, "Potion collected", 255, 255, 0);
		}
	}

	if (hud.sound)
		al_play_sample(i.se, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	i = itemList[itemSize - 1];
	itemSize--;
}


void fountain_draw(Fountain f) {
	if (f.reloadFreqCheck < f.reloadFreq)
		al_draw_bitmap(f.image_inactive, f.x, f.y - (al_get_bitmap_height(f.image_inactive) - 32), NULL);
	else
		al_draw_bitmap(f.image_active, f.x, f.y - (al_get_bitmap_height(f.image_active) - 32), NULL);
}

void fountain_reward(Fountain &f) {

	f.reloadFreqCheck++;

	float xDiff = abs(player.x - f.x);
	float yDiff = abs(player.y - f.y);
	if (sqrt(xDiff * xDiff + yDiff * yDiff) > 56)
		return;
	if (f.reloadFreqCheck < f.reloadFreq)
		return;
	if (player.inventory.gold < f.cost)
		return;

	al_get_keyboard_state(&keyState);
	if (!al_key_down(&keyState, KEY_SELECT))
		return;

	f.reloadFreqCheck = 0;

	// remove and add to event log
	player.inventory.gold -= f.cost;
	event_log_add(hud, "Sacrificed G to fountain", 128, 128, 255);

	// play sound
	if (hud.sound)
		al_play_sample(f.se, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	int i = rand() % 1000;

	// draw item
	if (i < f.itemChance * 1000) {
		int j = rand() % (ITEM_SIZE - 1) + 1;
		itemSize++;
		if (j == POTION) {
			itemList[itemSize - 1] = new_item(ITEM, POTION, al_load_bitmap("Image/Object/potion.png"), f.x, f.y);
			event_log_add(hud, "Summoned 1 potion", 128, 128, 255);
		}
	}
	// draw weapon
	else if (i < (f.weaponChance + f.itemChance) * 1000) {
		int j;
		do {
			j = rand() % (WEAPON_SIZE - 1) + 1;
		} while (player.weapon.name == weapons[j].name);
		itemSize++;
		if (j == SWORD) {
			itemList[itemSize - 1] = new_item(WEAPON, SWORD, al_load_bitmap("Image/Object/sword.png"), f.x, f.y);
			event_log_add(hud, "Summoned 1 sword", 128, 128, 255);
		}
		if (j == CELLO) {
			itemList[itemSize - 1] = new_item(WEAPON, CELLO, al_load_bitmap("Image/Object/cello.png"), f.x, f.y);
			event_log_add(hud, "Summoned 1 cello", 128, 128, 255);
		}
	}
	// no item
	else {
	}

}


int map_at(Map m, int x, int y) {
	return m.grid[y * m.w + x];
}

void map_draw(Map m) {

	for (int i = 0; i < m.h; i++) {
		for (int j = 0; j < m.w; j++) {

			if (in_camera_range(j * 32, i * 32, camera)) {
				// set frame x and frame y
				int frameX = m.tileset.frame;
				int frameY = map_at(m, j, i);
				// draw tile
				al_draw_bitmap_region(m.tileset.image, frameX * 32, frameY * 32, 32, 32, j * 32, i * 32, NULL);
			}

		}
	}
}

void map_draw_parallax(Map &m, float opacity) {

	// change wind
	m.parallax.x -= m.windX;
	m.parallax.y -= m.windY;

	// create bX and bY
	int bX = (m.parallax.x + GAME_WIDTH) % GAME_WIDTH;
	int bY = (m.parallax.y + GAME_HEIGHT) % GAME_HEIGHT;

	// draw layer 1
	if (m.parallax.img1 != NULL) {
		al_draw_tinted_bitmap(m.parallax.img1, al_map_rgba_f(opacity, opacity, opacity, opacity), 0, 0, NULL);
	}

	// draw layer 2 (moving)
	if (m.parallax.img2 != NULL) {
		al_draw_tinted_bitmap(m.parallax.img2, al_map_rgba_f(opacity, opacity, opacity, opacity), bX, bY, NULL);
		al_draw_tinted_bitmap(m.parallax.img2, al_map_rgba_f(opacity, opacity, opacity, opacity), bX + GAME_WIDTH, bY, NULL);
		al_draw_tinted_bitmap(m.parallax.img2, al_map_rgba_f(opacity, opacity, opacity, opacity), bX, bY + GAME_HEIGHT, NULL);
		al_draw_tinted_bitmap(m.parallax.img2, al_map_rgba_f(opacity, opacity, opacity, opacity), bX + GAME_WIDTH, bY + GAME_HEIGHT, NULL);
	}

	// draw layer 3
	if (m.parallax.img3 != NULL) {
		al_draw_tinted_bitmap(m.parallax.img3, al_map_rgba_f(opacity, opacity, opacity, opacity), 0, 0, NULL);
	}

}


bool covering_tile(Character c, Map m) {
	// set tile ids
	int tl = map_at(m, floor((c.x + 4) / 32.0), floor((c.y + 4) / 32.0));
	int tr = map_at(m, ceil((c.x - 4) / 32.0), floor((c.y + 4) / 32.0));
	int bl = map_at(m, floor((c.x + 4) / 32.0), ceil((c.y - 4) / 32.0));
	int br = map_at(m, ceil((c.x - 4) / 32.0), ceil((c.y - 4) / 32.0));

	if (m.tileset.data[tl] == 1) // top-left
		return true;
	if (m.tileset.data[tr] == 1) // top-right
		return true;
	if (m.tileset.data[bl] == 1) // bottom-left
		return true;
	if (m.tileset.data[br] == 1) // bottom-right
		return true;

	return false;
}

bool covering_character(Character c) {
	if (player.x != c.x || player.y != c.y) {
		if (abs(c.x - player.x) < 32 && abs(c.y - player.y) < 32)
			return true;
	}

	// loop through enemies
	for (int i = 0; i < enemySize; i++) {
		if (enemyList[i].x != c.x || enemyList[i].y != c.y) {
			if (abs(c.x - enemyList[i].x) < 32 && abs(c.y - enemyList[i].y) < 32)
				return true;
		}
	}
	return false;
}

bool covering_fountain(Character c) {
	// loop through fountains
	for (int i = 0; i < fountainSize; i++) {
		if (fountainList[i].x != c.x || fountainList[i].y != c.y) {
			if (abs(c.x - fountainList[i].x) < 32 && abs(c.y - fountainList[i].y) < 32)
				return true;
		}
	}

	return false;
}

bool out_of_map(Character c, Map m) {
	if (c.x < 0 || c.y < 0) // up and left boundariew
		return true;
	if (c.x >= (m.w - 1) * 32 || c.y >= (m.h - 1) * 32) // down and right boundaries
		return true;
	return false;
}


void hud_title_select(Hud &h) {

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (!al_key_down(&keyState, KEY_SELECT)) {
		h.indexEnterFreqCheck = 999;
		return;
	}

	h.indexEnterFreqCheck++;
	if (h.indexEnterFreqCheck < h.indexEnterFreq)
		return;

	// if title scene
	if (scene == TITLE) {
		switch (h.indexSelected) {
		case 0:
			scene = GAME;
			al_stop_sample(&bgmTitleId);
			al_play_sample(bgmNormal, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmNormalId);
			break;
		case 1:
			scene = INSTRUCTIONS;
			break;
		case 2:
			scene = CREDITS;
			hud.creditsY = 0;
			break;
		case 3:
			exitGame = true;
			break;
		}
	}

	// if credits or instructions
	else if (scene == CREDITS || scene == INSTRUCTIONS) {
		scene = TITLE;
	}

	// play sound
	if (h.sound)
		al_play_sample(h.se_select, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	h.indexEnterFreqCheck = 0;
	h.indexSelected = 0;
}

void hud_index_scroll(Hud &h, int scene) {

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (!al_key_down(&keyState, KEY_UP) && !al_key_down(&keyState, KEY_DOWN)) {
		h.indexSelectFreqCheck = 999;
		return;
	}

	h.indexSelectFreqCheck++;
	if (h.indexSelectFreqCheck < h.indexSelectFreq)
		return;

	if (al_key_down(&keyState, KEY_UP)) {
		if (h.sound)
			al_play_sample(h.se_scroll, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		h.indexSelected--;
	}
	if (al_key_down(&keyState, KEY_DOWN)) {
		if (h.sound)
			al_play_sample(h.se_scroll, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		h.indexSelected++;
	}
	if (scene == MENU) {
		h.indexSelected += MENU_SELECTION_NUM;
		h.indexSelected %= MENU_SELECTION_NUM;
	}
	if (scene == TITLE) {
		h.indexSelected += TITLE_SELECTION_NUM;
		h.indexSelected %= TITLE_SELECTION_NUM;
	}
	h.indexSelectFreqCheck = 0;
}

void hud_credits_scroll(Hud &h) {

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (!al_key_down(&keyState, KEY_UP) && !al_key_down(&keyState, KEY_DOWN)) {
		h.indexSelectFreqCheck = 999;
		return;
	}

	h.indexSelectFreqCheck++;
	if (h.indexSelectFreqCheck < h.indexSelectFreq)
		return;

	if (al_key_down(&keyState, KEY_UP)) {
		h.creditsY += h.creditsScrollrate;
	}
	if (al_key_down(&keyState, KEY_DOWN)) {
		h.creditsY -= h.creditsScrollrate;
	}

	if (h.creditsY < -(al_get_bitmap_height(h.credits) - GAME_HEIGHT))
		h.creditsY = -(al_get_bitmap_height(h.credits) - GAME_HEIGHT);
	if (h.creditsY > 0)
		h.creditsY = 0;

}


void hud_menu_toggle(Hud &h) {

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (!al_key_down(&keyState, KEY_CANCEL)) {
		h.indexToggleFreqCheck = 999;
		return;
	}

	h.indexToggleFreqCheck++;
	if (h.indexToggleFreqCheck < h.indexToggleFreq)
		return;

	if (scene == GAME) {
		if (h.sound)
			al_play_sample(h.se_select, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		scene = MENU;
	}
	else if (scene == MENU) {
		if (h.sound)
			al_play_sample(h.se_cancel, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		scene = GAME;
	}

	h.indexToggleFreqCheck = 0;
	h.indexSelected = 0;

}

void hud_menu_select(Hud &h) {

	// get keyboard state
	al_get_keyboard_state(&keyState);

	if (!al_key_down(&keyState, KEY_SELECT)) {
		h.indexEnterFreqCheck = 999;
		return;
	}

	h.indexEnterFreqCheck++;
	if (h.indexEnterFreqCheck < h.indexEnterFreq)
		return;

	if (scene == MENU) {
		switch (h.indexSelected) {
		case 0:
			h.music = !h.music;
			hud_change_menu_music(hud);
			break;
		case 1:
			h.sound = !h.sound;
			break;
		case 2:
			scene = MENU_CONTROLS;
			break;
		case 3:
			h.framerate = !h.framerate;
			break;
		case 4:
			scene = GAME;
			break;
		case 5:
			restartGame = true;
			break;
		case 6:
			exitGame = true;
			break;
		}
	}
	else if (scene == MENU_CONTROLS) {

		scene = MENU;

	}

	if (h.sound)
		al_play_sample(h.se_select, SE_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	h.indexEnterFreqCheck = 0;
	h.indexSelected = 0;
}

void hud_update_frame_rate(Hud &h) {
	float temp = get_frame_rate();

	if (!h.framerate && !developperMode) {
		h.framerateFreqCheck = 999;
		return;
	}

	h.framerateFreqCheck++;
	if (h.framerateFreqCheck >= h.framerateFreq) {
		framerate = temp;
		h.framerateFreqCheck = 0;
	}
}

void hud_change_menu_music(Hud h) {
	if (h.music) {
		if (!fightingBoss)
			al_play_sample(bgmNormal, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmNormalId);
		else
			al_play_sample(bgmBoss, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmBossId);
	}
	else {
		al_stop_sample(&bgmNormalId);
		al_stop_sample(&bgmBossId);
		al_stop_sample(&bgmTitleId);
		al_stop_sample(&bgmWinId);
		al_stop_sample(&bgmLoseId);
	}
}


void hud_draw_title(Hud h) {
	al_draw_bitmap(h.title_back, 0, 0, NULL);
	al_draw_bitmap(h.title_title, 0, 0, NULL);
	switch (h.indexSelected) {
	case 0:
		al_draw_bitmap(h.title_play, 0, 0, NULL);
		break;
	case 1:
		al_draw_bitmap(h.title_instructions, 0, 0, NULL);
		break;
	case 2:
		al_draw_bitmap(h.title_credits, 0, 0, NULL);
		break;
	case 3:
		al_draw_bitmap(h.title_exit, 0, 0, NULL);
		break;
	}
}

void hud_draw(Hud &h, Character p) {
	int h_oX = 80, h_w = 256;
	int m_oX = 80, m_w = 176;

	al_draw_bitmap(h.hud_back, 0, 0, NULL);
	al_draw_bitmap_region(h.hud_hpBar, 0, 0, h_oX + h_w * ((float)p.hp / p.maxHp), GAME_HEIGHT, 0, 0, NULL);
	al_draw_bitmap_region(h.hud_mpBar, 0, 0, m_oX + m_w * ((float)p.mp / p.maxMp), GAME_HEIGHT, 0, 0, NULL);
	al_draw_bitmap(h.hud_info, 0, 0, NULL);

	if (player.inventory.potions <= 0) {
		al_draw_filled_rectangle(564, 24, 604, 64, al_map_rgba_f(0, 0, 0, 0.5));
	}
	else if (player.inventory.potionFreqCheck < player.inventory.potionFreq) {
		al_draw_filled_rectangle(564, 24 + ((float)player.inventory.potionFreqCheck / player.inventory.potionFreq) * 40, 604, 64, al_map_rgba_f(0, 0, 0, 0.5));
	}

	al_draw_textf(h.font_hp, al_map_rgb(255, 255, 255), 100, 26, ALLEGRO_ALIGN_LEFT, "%d / %d", int(p.hp), int(p.maxHp));
	al_draw_textf(h.font_mp, al_map_rgb(255, 255, 255), 100, 52, ALLEGRO_ALIGN_LEFT, "%d%c", int(p.mp / p.maxMp * 100), '%');
	al_draw_textf(h.font_potions, al_map_rgb(255, 255, 255), 568, 24, ALLEGRO_ALIGN_LEFT, "%d", p.inventory.potions);
	al_draw_textf(h.font_gold, al_map_rgb(255, 255, 255), 548, 30, ALLEGRO_ALIGN_RIGHT, "%dG", p.inventory.gold);
	if (hud.framerate || developperMode)
		al_draw_textf(h.font_framerate, al_map_rgb(255, 0, 0), 632, 450, ALLEGRO_ALIGN_RIGHT, "%.1fFPS", int(round(framerate * 10)) * 0.1);

	// set event log opacity
	h.eventLogFadeFreqCheck++;
	float eventLogOpacity = 1 - (float)h.eventLogFadeFreqCheck / h.eventLogFadeFreq;
	if (h.eventLogFadeFreqCheck >= h.eventLogFadeFreq) {
		eventLogOpacity = 0;
		event_log_erase(h);
	}

	// draw event log
	for (int i = 0; i < EVENT_LOG_MAX_SIZE; i++) {
		if (strcmp(h.eventLog[i], "") != 0) {
			ALLEGRO_COLOR color = al_map_rgba_f(eventLogOpacity * h.eventLogColor[i][0] / 255.0, eventLogOpacity * h.eventLogColor[i][1] / 255.0, eventLogOpacity * h.eventLogColor[i][2] / 255.0, eventLogOpacity);
			al_draw_tinted_bitmap(h.hud_eventLog, al_map_rgba_f(eventLogOpacity, eventLogOpacity, eventLogOpacity, eventLogOpacity), 16, 336 + i * 32, NULL);
			al_draw_textf(h.font_eventLog, color, 32, 336 + 8 + i * 32, ALLEGRO_ALIGN_LEFT, "%s", h.eventLog[i]);
		}
	}
}

void hud_draw_popup(Hud &h) {
	if (player.dead) {
		al_draw_bitmap(h.text_gameover, 0, 0, NULL);
	}
	else if (bossKilled) {
		al_draw_bitmap(h.text_win, 0, 0, NULL);
	}
	else if (h.bossFightPopup) {
		float opacity = 1 - (float)h.popupDurCheck / h.popupDur;
		al_draw_tinted_bitmap(h.text_bossFight, al_map_rgba_f(opacity, opacity, opacity, opacity), 0, 0, NULL);
		h.popupDurCheck++;
		if (h.popupDurCheck >= h.popupDur) {
			h.bossFightPopup = false;
		}
	}
}

void hud_draw_menu(Hud h) {

	if (scene == MENU) {
		al_draw_bitmap(h.menu_back, 0, 0, NULL);
		switch (h.indexSelected) {
		case 0:
			al_draw_bitmap(h.menu_music, 0, 0, NULL);
			break;
		case 1:
			al_draw_bitmap(h.menu_sound, 0, 0, NULL);
			break;
		case 2:
			al_draw_bitmap(h.menu_controls, 0, 0, NULL);
			break;
		case 3:
			al_draw_bitmap(h.menu_framerate, 0, 0, NULL);
			break;
		case 4:
			al_draw_bitmap(h.menu_cancel, 0, 0, NULL);
			break;
		case 5:
			al_draw_bitmap(h.menu_restart, 0, 0, NULL);
			break;
		case 6:
			al_draw_bitmap(h.menu_exit, 0, 0, NULL);
			break;
		}
	}
	else if (scene == MENU_CONTROLS) {
		al_draw_bitmap(h.instructions, 0, 0, NULL);
	}

}


void event_log_add(Hud &h, char str[], int r, int g, int b) {

	for (int i = EVENT_LOG_MAX_SIZE - 2; i >= 0; i--) {
		strcpy(h.eventLog[i + 1], h.eventLog[i]);
		h.eventLogColor[i + 1][0] = h.eventLogColor[i][0];
		h.eventLogColor[i + 1][1] = h.eventLogColor[i][1];
		h.eventLogColor[i + 1][2] = h.eventLogColor[i][2];
	}

	strcpy(h.eventLog[0], str);
	h.eventLogColor[0][0] = r;
	h.eventLogColor[0][1] = g;
	h.eventLogColor[0][2] = b;
	h.eventLogFadeFreqCheck = 0;
}

void event_log_erase(Hud &h) {
	for (int i = 0; i < EVENT_LOG_MAX_SIZE; i++) {
		strcpy(h.eventLog[i], "");
	}
}


void camera_update(Camera &c, Character p) {
	c.x = -(GAME_WIDTH / 2) + p.x + 16;
	c.y = -(GAME_HEIGHT / 2) + p.y + 16;

	if (c.x < 0)
		c.x = 0;
	if (c.y < 0)
		c.y = 0;
	if (c.x + GAME_WIDTH > map.w * 32)
		c.x = map.w * 32 - GAME_WIDTH;
	if (c.y + GAME_HEIGHT> map.h * 32)
		c.y = map.h * 32 - GAME_HEIGHT;
}

void camera_move(ALLEGRO_TRANSFORM t, Camera c) {
	if (camera.freeze)
		return;

	al_identity_transform(&t);
	al_translate_transform(&t, -c.x, -c.y); // translate -cameraX, -cameraY
	al_use_transform(&t);
}

void camera_reset(ALLEGRO_TRANSFORM t) {
	al_identity_transform(&t);
	al_use_transform(&t);
}

bool in_camera_range(int x, int y, Camera c) {

	int buffer = 64;

	if (c.x > x + buffer)
		return false;
	if (c.y > y + buffer)
		return false;
	if (x - c.x > GAME_WIDTH + buffer)
		return false;
	if (y - c.y > GAME_HEIGHT + buffer)
		return false;
	return true;
}


void tileset_animate(Tileset &t) {
	// update animation check
	t.animFreqCheck++;
	// if time to update frame, update frame
	if (t.animFreqCheck >= t.animFreq) {
		t.frame++;
		t.frame %= t.w;
		t.animFreqCheck = 0;
	}
}



void boss_fight_start() {
	if (fightingBoss)
		return;

	fightingBoss = true;

	map.grid[63 * map.w + 9] = 20;
	map.grid[63 * map.w + 10] = 20;
	map.grid[63 * map.w + 11] = 20;
	map.grid[62 * map.w + 9] = 18;
	map.grid[62 * map.w + 10] = 18;
	map.grid[62 * map.w + 11] = 18;
	hud.bossFightPopup = true;
	hud.popupDurCheck = 0;

	if (hud.music) {
		al_stop_sample(&bgmNormalId);
		al_stop_sample(&bgmBossId);
		al_stop_sample(&bgmTitleId);
		al_stop_sample(&bgmWinId);
		al_stop_sample(&bgmLoseId);
		al_play_sample(bgmBoss, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgmBossId);
	}

}


