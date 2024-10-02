
#include "src/utils.c"
#include "src/data.c"
#include "src/dialog_flags.c"
#include "src/animate.c"
#include "src/game.c"
#include "src/input.c"
#include "src/init.c"
#include "src/audio.c"



// Audio_Player* getAudioPlayer(Entity* background);
// void fadeOutAudio(Audio_Player* audio, float32 duration);
// void fadeInAudio(Audio_Player* audio, float32 duration);


// TODO:
// - UXSTATEs - inGame, inMenu, inInventory, Pause, 
// - Click to Move
// - VIEWS/LAYERS - Background (1,2,3 for parallax), Objects (including Player and hotspots, walkboxes etc), 
//		Foreground, Cursors and Context, Inventory, Journal, Zoom-Ins, Map, Menus
// - Room loading, doors, 
// - Room script? click script, scripting lang?
// - Animations
// - Inventory item combining
// - Inventory item use on environment
// - Dialogue System - 
// - Settings and Save System

int entry(int argc, char **argv) 
{
	// :init OS stuff
	window.title = STR("Point and Click");
	if (window.allow_resize)
	{
		window.allow_resize = !window.allow_resize;
	} 

	Gfx_Font *font = load_font_from_disk(STR("jamAssets/fonts/NotJamOldStyle11.ttf"), get_heap_allocator());
	assert(font, "Failed loading font");
	const u32 fontHeight = 48;
	render_atlas_if_not_yet_rendered(font, fontHeight, 'A'); 
	Vector2 textScaling = v2(0.2, 0.2);
	Vector2 textScalingBig = v2(0.5, 0.5);
	Vector2 textScalingSml = v2(0.1, 0.1);

	window.point_width = 800;
	window.point_height = 600;
	float zoom = 4.0;
	//u8 screenSize = 3;

	// below is the window offest from bottom left of screen - maybe get screen dimensions and center it.
	window.point_x = 300;
	window.point_y = 300;
	window.clear_color = COLOR_RED; // background color

	// disable windows cursor
	ShowCursor(false);

	// :Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	#include "src/dialog.c"
	
	loadAudio(au_train, STR("jamAssets/audio/train2.ogg"));
	loadAudio(au_song1, STR("jamAssets/audio/Kool-Kats.ogg"));
	loadAudio(au_song2, STR("jamAssets/audio/Hard-Boiled.ogg"));
	loadAudio(au_song3, STR("jamAssets/audio/I-Knew-a-Guy.ogg"));
	loadAudio(au_song4, STR("jamAssets/audio/On-the-Cool-Side.ogg"));
	loadAudio(au_song5, STR("jamAssets/audio/Walking-Along.ogg"));
	loadAudio(au_song6, STR("jamAssets/audio/Clean-Soul.ogg"));
	loadAudio(au_song7, STR("jamAssets/audio/Dances-and-Dames.ogg"));
	//loadAudio(a_sound1, STR("jamAssets/audio/Kool-Kats.ogg"));
	//loadAudio(a_sound2, STR("jamAssets/audio/Kool-Kats.ogg"));

	Audio_Player* songPlayer = audio_player_get_one();
	audio_player_set_source(songPlayer, *getAudioSource(au_song1));
	audio_player_set_state(songPlayer, AUDIO_PLAYER_STATE_PAUSED);
    audio_player_set_looping(songPlayer, false);

	Audio_Player* bgNoisePlayer = audio_player_get_one();
	audio_player_set_source(bgNoisePlayer, *getAudioSource(au_train));
	audio_player_set_state(bgNoisePlayer, AUDIO_PLAYER_STATE_PAUSED);
    audio_player_set_looping(bgNoisePlayer, false);

	// Audio_Player* clipPlayer = audio_player_get_one();
	// audio_player_set_source(clipPlayer, getAudioSource(au_song1));
	// audio_player_set_state(clipPlayer, AUDIO_PLAYER_STATE_PAUSED);
    // audio_player_set_looping(clipPlayer, true);
	
	// :loadWalkboxes // [sleeper] : [cargo]-[luggage]-(0,0)[dining]-[hallway]-[lounge]
	loadWalkbox(w_dining_1, boxMake(v2(70.0, 97.0), v2(350.0, 123.0), false, true, false, false));
	loadWalkbox(w_dining_2, boxMake(v2(350.0, 97.0), v2(400.0, 123.0),true, true, true, false));
	loadWalkbox(w_dining_3, boxMake(v2(350.0, 123.0), v2(400.0, 147.0), false, false, false, true));
	loadWalkbox(w_dining_4, boxMake(v2(400.0, 97.0), v2(530.0, 123.0), true, false, false, false));
	
	loadWalkbox(w_dining_5, boxMake(v2(400.0, 135.0), v2(412.0, 147.0), true, true, false, true));
	loadWalkbox(w_dining_6, boxMake(v2(412.0, 135.0), v2(515.0, 147.0), true, false, false, false));
	loadWalkbox(w_dining_7, boxMake(v2(400.0, 123.0), v2(412.0, 135.0), true, false, true, true));


	loadWalkbox(w_luggage_1, boxMake(v2(-220.0, 123.0), v2(-135.0, 150.0), false, false, false, true));
	loadWalkbox(w_luggage_2, boxMake(v2(-330.0, 97.0), v2(-220.0, 123.0), false, true, false, false));
	loadWalkbox(w_luggage_3, boxMake(v2(-220.0, 97.0), v2(-135.0, 123.0), true, true, true, false));
	loadWalkbox(w_luggage_4, boxMake(v2(-135.0, 97.0), v2(-70.0, 123.0), true, false, false, false));

	loadWalkbox(w_hallway_1, boxMake(v2(670.0, 97.0), v2(1130.0, 119.0), false, false, false, false));

	loadWalkbox(w_lounge_1, boxMake(v2(1300.0, 97.0), v2(1700.0, 123.0), true, true, true, false));
	loadWalkbox(w_lounge_2, boxMake(v2(1270.0, 97.0), v2(1300.0, 123.0), false, true, false, false));
	loadWalkbox(w_lounge_3, boxMake(v2(1700.0, 97.0), v2(1730.0, 123.0), true, false, false, false));
	loadWalkbox(w_lounge_4, boxMake(v2(1300.0, 123.0), v2(1700.0, 150.0), false, false, false, true));

	loadWalkbox(w_cargo_1, boxMake(v2(-730.0, 97.0), v2(-470.0, 150.0), false, false, false, false));

	loadWalkbox(w_sleeper_1, boxMake(v2(-1130.0, 97.0), v2(-955.0, 123.0), false, true, false, false));
	loadWalkbox(w_sleeper_2, boxMake(v2(-955.0, 97.0), v2(-910.0, 105.0), true, false, true, false));
	loadWalkbox(w_sleeper_3, boxMake(v2(-955.0, 105.0), v2(-910.0, 123.0), true, true, false, true));
	loadWalkbox(w_sleeper_4, boxMake(v2(-910.0, 105.0), v2(-870.0, 123.0), true, false, false, false));

	// :loadCursors
	world->isHWCursor = true;
	loadCursor(c_click, STR("jamAssets/cursors/click.png"));
	loadCursor(c_grab, STR("jamAssets/cursors/grab.png"));
	loadCursor(c_look, STR("jamAssets/cursors/look.png"));
	loadCursor(c_talk, STR("jamAssets/cursors/talk2.png"));
	loadCursor(c_left, STR("jamAssets/cursors/left.png"));
	loadCursor(c_right, STR("jamAssets/cursors/right.png"));
	loadCursor(c_drag, STR("jamAssets/cursors/finger.png"));
	loadCursor(c_up, STR("jamAssets/cursors/up.png"));
	loadCursor(c_down, STR("jamAssets/cursors/down.png"));
	loadCursor(c_hot, STR("jamAssets/cursors/hot.png"));


	// :loadCharacters
	loadSprite(s_player, STR("jamAssets/characters/Adaline-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 8, 5);
	loadSprite(s_ch_conductor, STR("jamAssets/characters/Conductor-Idle-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_reporter, STR("jamAssets/characters/Reporter-Idle-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_baron, STR("jamAssets/characters/Baron-Idle-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_detective, STR("jamAssets/characters/Detective-Idle-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_professor, STR("jamAssets/characters/Professor-Idle-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_starlet, STR("jamAssets/characters/Starlet-Sheet.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);
	loadSprite(s_ch_valet, STR("jamAssets/characters/Valet-Idle-Sheet-half.png"), v2(32.0, 64.0), v2(16.0, 0.0), true, 4, 1);

	// :loadPortraits
	loadSprite(s_po_player, STR("jamAssets/portraits/MCPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_baron, STR("jamAssets/portraits/BaronPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_reporter, STR("jamAssets/portraits/ReporterPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_detective, STR("jamAssets/portraits/DetectivePortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_conductor, STR("jamAssets/portraits/ConductorPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_professor, STR("jamAssets/portraits/ProfessorPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_valet, STR("jamAssets/portraits/ValetPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_starlet, STR("jamAssets/portraits/StarletPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);

	// :loadUI
	loadSprite(s_ui_dialog, STR("jamAssets/portraits/UI-Dialog.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_ui_inventory, STR("jamAssets/portraits/UI-Inventory.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);

	// :loadItems
	loadSprite(s_item_coupon, STR("jamAssets/objects/coupon.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_drink, STR("jamAssets/objects/drink.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_headshot, STR("jamAssets/objects/headshot.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_key, STR("jamAssets/objects/wallkey.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_obj_bar, STR("jamAssets/objects/bar.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);


	// :loadBackgrounds
	loadSprite(s_bg_dining, STR("jamAssets/rooms/DiningCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_hallway, STR("jamAssets/rooms/HallwayBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_lounge, STR("jamAssets/rooms/LoungeCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_luggage, STR("jamAssets/rooms/LuggageClosetBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_cargo, STR("jamAssets/rooms/CargoBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_sleeper, STR("jamAssets/rooms/MCSleeperCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_menu, STR("jamAssets/rooms/BlankBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_bg_settings, STR("jamAssets/rooms/BlankBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);


	// :createBackgrounds - these don't need to be entities in final hopefully
	Entity* bgDining = createEntity(t_background, s_bg_dining, i_nil, v2(0, 0), null_string, false, 0);
	Entity* bgHallway = createEntity(t_background, s_bg_hallway, i_nil, v2(600.0, 0), null_string, false, 0);
	Entity* bgLounge = createEntity(t_background, s_bg_lounge, i_nil, v2(1200.0, 0), null_string, false, 0);
	Entity* bgLuggage = createEntity(t_background, s_bg_luggage, i_nil, v2(-400, 0), null_string, false, 0);
	Entity* bgCargo = createEntity(t_background, s_bg_cargo, i_nil, v2(-800, 0), null_string, false, 0);
	Entity* bgSleeper = createEntity(t_background, s_bg_sleeper, i_nil, v2(-1200, 0), null_string, false, 0);
  	// [sleeper] : [cargo]-[luggage]-(0,0)[dining]-[hallway]-[lounge]

	// :Menus
	Entity* bgMenu = createEntity(t_background, s_bg_menu, i_nil, v2(1800, 0), null_string, false, 0);
	Entity* bgSettings = createEntity(t_background, s_bg_settings, i_nil, v2(2200, 0), null_string, false, 0);

	// :createEntities and Objects
	Entity* player = createEntity(t_player, s_player, i_nil, v2(175.0, 106.0), null_string, false, 0);
	Entity* conductor = createEntity(t_npc, s_ch_conductor, i_nil, v2(-310, 110), STR("Conductor"), true, 0);
	Entity* reporter = createEntity(t_npc, s_ch_reporter, i_nil, v2(1100, 110), STR("Reporter"), true, 0);
	Entity* baron = createEntity(t_npc, s_ch_baron, i_nil, v2(110, 120), STR("Baron"), true, 0);
	Entity* detective = createEntity(t_npc, s_ch_detective, i_nil, v2(805, 115), STR("Detective"), true, 0);
	Entity* professor = createEntity(t_npc, s_ch_professor, i_nil, v2(1390, 140), STR("Professor"), true, 0);
	Entity* starlet = createEntity(t_npc, s_ch_starlet, i_nil, v2(1665, 140), STR("Starlet"), true, 0);
	Entity* valet = createEntity(t_npc, s_ch_valet, i_nil, v2(468, 145), STR("Valet"), true, 0);

	// :createItems
	Entity* coupon = createEntity(t_object, s_item_coupon, i_coupon, v2(-900.0, 106.0), STR("Coupon"), true, 0);
	// Entity* drink = createEntity(t_item, s_item_drink, i_drink, v2(-100, -100), STR("Fancy Cocktail"), true, 0);
	// Entity* headshot = createEntity(t_item, s_item_headshot, i_headshot, v2(-100, -100), STR("Headshot of Starlet"), true, 0);
	Entity* key = createEntity(t_object, s_item_key, i_key, v2(521.5, 191.0), STR("Brass Key"), true, 0);

	//Entity* key = createEntity(t_object, s_item_key, i_key, v2(-100, -100), STR("Brass Key"), true, 0);
	Entity* bar = createEntity(t_foreground, s_obj_bar, i_nil, v2(411, 128), STR(""), false, 0);

	// :createObjects - HotSpots/Walkboxes/Doors etc...
	Quad tempQuad = makeQuad(v2(412.0, 127.0), v2(524.0, 127.0), v2(524.0, 176.0), v2(412.0, 176.0));
	// createObject(o_bartender, tempQuad, ot_npc, v2(435.0, 122.0), v2(0,0), bgDining, c_talk);
	// createObject(o_bar, tempQuad, ot_object, v2(0, 0), v2(0,0), bgDining, c_click);

	// :Menu and Settings Screen Buttons
	Vector2 menuPos = v2(1960.0, 175.0);
	tempQuad = makeQuad(menuPos, v2(menuPos.x + 80, menuPos.y), v2(menuPos.x + 80, menuPos.y + 16), v2(menuPos.x, menuPos.y + 16));
	createObject(o_newgame, tempQuad, ot_newgame, v2(0.0, 0.0), v2(-1000.0, 106.0), bgSleeper, c_hot);
	menuPos = v2(1960.0, 145.0);
	tempQuad = makeQuad(menuPos, v2(menuPos.x + 80, menuPos.y), v2(menuPos.x + 80, menuPos.y + 16), v2(menuPos.x, menuPos.y + 16));
	createObject(o_settings, tempQuad, ot_settings, v2(0.0, 0.0), v2(0,0), bgSettings, c_hot);
	menuPos = v2(1960.0, 115.0);
	tempQuad = makeQuad(menuPos, v2(menuPos.x + 80, menuPos.y), v2(menuPos.x + 80, menuPos.y + 16), v2(menuPos.x, menuPos.y + 16));
	createObject(o_quit, tempQuad, ot_quit, v2(0.0, 0.0), v2(0,0), null, c_hot);


	// :createDoors
	// DiningRoom
	tempQuad = makeQuad(v2(40.0, 95.0), v2(70.0, 95.0), v2(70.0, 195.0), v2(40.0, 195.0));
	createObject(o_door_diningL, tempQuad, ot_door, v2(71.0, 106.0), v2(-71.0, 106.0), bgLuggage, c_left);
	tempQuad = makeQuad(v2(530.0, 95.0), v2(560.0, 95.0), v2(560.0, 195.0), v2(530.0, 195.0));
	createObject(o_door_diningR, tempQuad, ot_door, v2(529.0, 106.0), v2(671, 106.0), bgHallway, c_right);

	// Hallway
	tempQuad = makeQuad(v2(640.0, 95.0), v2(670.0, 95.0), v2(670.0, 195.0), v2(640.0, 195.0));
	createObject(o_door_hallwayL, tempQuad, ot_door, v2(671.0, 106.0), v2(529.0, 106.0), bgDining, c_left);
	tempQuad = makeQuad(v2(1130.0, 95.0), v2(1160.0, 95.0), v2(1160.0, 195.0), v2(1130.0, 195.0));
	createObject(o_door_hallwayR, tempQuad, ot_door, v2(1129.0, 106.0), v2(1271, 106.0), bgLounge, c_right);
	tempQuad = makeQuad(v2(940.0, 120.0), v2(976.0, 120.0), v2(976.0, 185.0), v2(940.0, 185.0));
	createObject(o_door_hallwayU, tempQuad, ot_door, v2(960.0, 115.0), v2(-885.0, 106.0), bgSleeper, c_up);

	// Lounge
	tempQuad = makeQuad(v2(1240.0, 95.0), v2(1270.0, 95.0), v2(1270.0, 195.0), v2(1240.0, 195.0));
	createObject(o_door_loungeL, tempQuad, ot_door, v2(1271.0, 106.0), v2(1129.0, 106.0), bgHallway, c_left);
	tempQuad = makeQuad(v2(1730.0, 95.0), v2(1760.0, 95.0), v2(1760.0, 195.0), v2(1730.0, 195.0));
	createObject(o_door_loungeR, tempQuad, ot_door, v2(1729.0, 106.0), v2(0.0, 0.0), null, c_right); // put check in for 0

	// Luggage
	tempQuad = makeQuad(v2(-360.0, 95.0), v2(-330.0, 95.0), v2(-330.0, 195.0), v2(-360.0, 195.0));
	createObject(o_door_luggageL, tempQuad, ot_door, v2(-329.0, 106.0), v2(-471.0, 106.0), bgCargo, c_left);
	tempQuad = makeQuad(v2(-70.0, 95.0), v2(-40.0, 95.0), v2(-40.0, 195.0), v2(-70.0, 195.0));
	createObject(o_door_luggageR, tempQuad, ot_door, v2(-71.0, 106.0), v2(71, 106.0), bgDining, c_right);

	// Cargo
	tempQuad = makeQuad(v2(-760.0, 95.0), v2(-730.0, 95.0), v2(-730.0, 195.0), v2(-760.0, 195.0));
	createObject(o_door_cargoL, tempQuad, ot_door, v2(-729.0, 106.0), v2(0.0, 0.0), null, c_left);
	tempQuad = makeQuad(v2(-470.0, 95.0), v2(-440.0, 95.0), v2(-440.0, 195.0), v2(-470.0, 195.0));
	createObject(o_door_cargoR, tempQuad, ot_door, v2(-471.0, 106.0), v2(-329, 106.0), bgLuggage, c_right);

	// Sleeper
	tempQuad = makeQuad(v2(-910.0, 95.0), v2(-860.0, 95.0), v2(-860.0, 105.0), v2(-910.0, 105.0));
	createObject(o_door_sleeperD, tempQuad, ot_door, v2(-885.0, 106.0), v2(960.0, 115.0), bgHallway, c_down);

	// add this to createEntity() ?
	conductor->interactPos.x = conductor->pos.x + 30.0; // + if facing right, - if facing left
	reporter->interactPos.x = reporter->pos.x - 30.0;
	baron->interactPos.x = baron->pos.x + 30.0;
	detective->interactPos.x = detective->pos.x - 30.0;
	starlet->interactPos.x = starlet->pos.x - 30.0;
	valet->interactPos.x = valet->pos.x - 30.0; // add pos.y
	professor->interactPos.x = professor->pos.x - 30.0;

	conductor->portraitID = s_po_conductor; 
	reporter->portraitID = s_po_reporter;
	baron->portraitID = s_po_baron;
	detective->portraitID = s_po_detective;
	starlet->portraitID = s_po_starlet;
	valet->portraitID = s_po_valet;
	professor->portraitID = s_po_professor;

	conductor->lookText = STR("It's the train conductor, I wonder what he is doing here.");
	reporter->lookText = STR("She looks like a reporter, I wonder what she is doing here.");
	baron->lookText = STR("That's quite the moustache, I wonder what he is doing here.");
	detective->lookText = STR("He looks like a policeman, I wonder what he is doing here.");
	starlet->lookText = STR("She looks familiar, I wonder what she is doing here.");
	valet->lookText = STR("That must be the valet.");
	professor->lookText = STR("He's a studious sort, I wonder what he is doing here.");

	conductor->dialogID = 1201;
	reporter->dialogID = 1301;
	baron->dialogID = 1401;
	detective->dialogID = 1501;
	starlet->dialogID = 1601;
	valet->dialogID = 1701;
	professor->dialogID = 1801;

	// :createInventoryItems
	loadInventoryItem(i_blank, STR("Blank"), STR("jamAssets/missingTexture.png"), true, 0);
	loadInventoryItem(i_coupon, STR("Drink Coupon"), STR("jamAssets/objects/coupon.png"), false, 0); // load this when needed? How?
	loadInventoryItem(i_key, STR("Brass Key"), STR("jamAssets/objects/key.png"), false, 0);
	loadInventoryItem(i_book, STR("Old Book"), STR("jamAssets/objects/book.png"), true, 0);
	loadInventoryItem(i_ticket, STR("Train Ticket"), STR("jamAssets/objects/ticket.png"), true, 0);
	loadInventoryItem(i_drink, STR("Fancy Cocktail"), STR("jamAssets/objects/drink.png"), false, 0);
	loadInventoryItem(i_headshot, STR("Autographed Photo"), STR("jamAssets/objects/headshot.png"), false, 0);

	// :init loop over inventory
	for (int i = 0; i < i_MAX; i++)
	{
		Item* item = &world->inventory[i];
		if (item->inInventory)
		{	
			pushInventory(item);
		}
	}

	// :init timers and fps etc
	float64 prevTime = os_get_elapsed_seconds();
	float64 secCounter = 0.0;
	s32 frameCounter = 0;
	s32 framerate = 0;
	int delayCounter = 0; // maybe add a data struct to keep a bunch of timers

	// :init game misc
	player->speed = 100.0;

	world->currentCursor = c_click;
	world->mouseActive = false;
	Matrix4 camera_xform = m4_scalar(1.0);
	Vector2 camera_pos = v2(player->pos.x, 150.0);
	
	world->uxStateID = ux_menu;
	world->itemOnCursor = null;
	world->currentBG = bgMenu;
	world->currentSongID = au_song1;
	// audio_player_set_state(songPlayer, AUDIO_PLAYER_STATE_PLAYING);
	songPlayer->config.volume = 0.2;
	bgNoisePlayer->config.volume = 0.2;

	world->playerText = STR("");
	world->dialogueBox = range2f_make(v2(30.0, 15.0), v2(370.0, 75.0));
	world->gameBox = range2f_make(v2(10.0, 76.0), v2(390.0, 300.0));
	world->debugOn = false;
	// world->screenFade = {0};

	world->activeSpeaker = player;
	player->pos = v2(-1000.0, 106.0);

	float32 textDuration = 2.0f;

	while (!window.should_close)
	{
		reset_temporary_storage();


		audio_player_set_state(songPlayer, AUDIO_PLAYER_STATE_PLAYING);
		if (audio_player_get_current_progression_factor(songPlayer) > 0.99)
		{
			// get duration from number of frames left...
			audio_player_transition_to_source(songPlayer, *getAudioSource(world->currentSongID), 1.0);
		}

		worldFrame = (WorldFrame){0};  // worldFrame is reset everyframe

		float64 time = os_get_elapsed_seconds();
		worldFrame.nowTime = time;
		float64 deltaTime = worldFrame.nowTime - prevTime;
		worldFrame.deltaTime = deltaTime;
		prevTime = worldFrame.nowTime;

		worldFrame.bg = world->currentBG; // = world.current bg or something...
		worldFrame.player = player; 

		// :camera - 
		// draw_frame.projection = m4_make_orthographic_projection(worldFrame.bg->scrollPos.x, worldFrame.bg->scrollPos.y, 0.0, 300.0, -1, 10);
		// worldFrame.world_proj = m4_make_orthographic_projection(0.0, screenWidth, 0.0, screenHeight, -1, 10);

		worldFrame.world_proj = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);
		Vector2 target_pos = player->pos;
		smoothCam(&camera_pos, target_pos, deltaTime, 4.0f);

		worldFrame.world_view = m4_identity;
		// translate into position
		worldFrame.world_view = m4_translate(worldFrame.world_view, v3(camera_pos.x, 150.0, 0));

		// scale the zoom
		worldFrame.world_view = m4_scale(worldFrame.world_view, v3(1.0/zoom, 1.0/zoom, 1.0));
		
		set_screen_space();
		worldFrame.mousePosScreen = getMouseCurrentProj();
		
		// :world stuff
		set_world_space();
		worldFrame.mousePosWorld = getMouseCurrentProj();
		Vector2 mousePosInput = v2(input_frame.mouse_x, input_frame.mouse_y);


		// :update loop over walkboxes
		for (int i = 0; i < w_MAX; i++)
		{
			Walkbox* box = &world->walkboxes[i];
			if (range2f_contains(box->box, player->pos)) worldFrame.activeWalkbox = box;
		}
			

		// :update loop over all entities
		{
			bool activeFound = false;
			for (int i = 0; i < MAX_ENTITY_COUNT; i++)
			{
				Entity* entity = &world->entities[i];

				// :mouseOver
				if (entity->isValid)
				{
					Sprite* sprite = getSprite(entity->spriteID);
					
					// check mouse in entity box - pull out to function
					// Range2f hotspot = range2f_make(v2_add(entity->pos, sprite->origin), v2_add(entity->pos, sprite->clickableSize));
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, entity->pos);
					if (entity->type != t_player && entity->type != t_background && entity->type != t_foreground)
					{
						if (fabsf(v2_dist(entity->pos, player->pos)) < entity->interactRadius) entity->isInRangeToInteract = true;
						else entity->isInRangeToInteract = false;

						if (range2f_contains(hotspot, worldFrame.mousePosWorld))
						{
							world->activeEntity = entity;
							world->mouseActive = true;
							activeFound = true;
							if (world->currentCursor != c_drag) world->currentCursor = c_hot;
							if (world->currentCursor != c_drag && entity->isInRangeToInteract) world->currentCursor = entity->hoverCursor;
						}
					}
				}	
			}
			if (!activeFound)
			{
				world->activeEntity = 0;
				if (world->activeObject == 0) world->mouseActive = false;
			} 
		}
		// :updateLoop over all objects (doors, static npcs, objects etc)
		{
			bool activeFound = false;
			for (int i = 1; i < o_MAX; i++)
			{
				Object* obj = &world->objects[i];
				if (obj->objectID != o_nil)
				{
					if (fabsf(v2_dist(obj->interactPos, player->pos)) < obj->interactRadius) obj->isInRangeToInteract = true;
					else obj->isInRangeToInteract = false;
					// if (isPointInConvexQuad(obj->quad, worldFrame.mousePosWorld))
					if (range2f_contains(range2f_make(obj->quad.q1, obj->quad.q3),worldFrame.mousePosWorld))
					{
						worldFrame.activeObject = obj;
						world->activeObject = obj;
						activeFound = true;
						world->mouseActive = true;
						if (world->currentCursor != c_drag) world->currentCursor = c_hot;
						if (world->currentCursor != c_drag && obj->isInRangeToInteract)
						{
							world->currentCursor = obj->hoverCursor;
						}
					}
				}
			}
			if (!activeFound)
			{
				world->activeObject = 0;
				if (world->activeEntity == 0) world->mouseActive = false;
			} 
		}
		// :update loop over inventory
		for (int i = 0; i < i_MAX; i++)
		{
			Item* item = &world->inventory[i];
			if (item->inInventory)
			{	
				Range2f hotspot = getHotSpot(item->size, item->origin);
				
			}
		}

		// :mouse input/hover/click stuff ( func inside the above?)
		// need to redo this to enter/exit hotspot I think, although it works for inventory for some reason.

		handleInput(player, world->activeEntity, world->activeItem, worldFrame.deltaTime);
		movePlayer(player, worldFrame.bg, worldFrame.nowTime, worldFrame.deltaTime);


		// :render loop over entities - pull out to function - z indexing??
		
		// :renderBackground
		if (worldFrame.bg != null)
		{
			Sprite* bgSprite = getSprite(worldFrame.bg->spriteID); 
			draw_image(bgSprite->image, worldFrame.bg->pos, bgSprite->size, COLOR_WHITE);
		}

		// :renderObjects
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid) // && onscreen?
			{

				if (e->type == t_object) 
				{	
					Sprite* itemSprite = getSprite(e->spriteID);
					Matrix4 xform = m4_scalar(1.0);
					xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
					xform = m4_translate(xform, v3(itemSprite->image->width * -0.5, 0.0, 0));
					// xform = m4_scale(xform, v3(0.5, 0.5, 1.0));
					draw_image_xform(itemSprite->image, xform, itemSprite->size, COLOR_WHITE);
				}
				// do doors, items?
			}
		}
		// :renderCharacters
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid) // && onscreen?
			{

				if (e->type == t_npc)
				{
					animate(e, worldFrame.nowTime, worldFrame.deltaTime);
				}
			}
		}
		// :renderPlayer
		animate(player, worldFrame.nowTime, worldFrame.deltaTime);

		// :renderForeground
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid) // && onscreen?
			{

				if (e->type == t_foreground) 
				{	
					Sprite* itemSprite = getSprite(e->spriteID);
					Matrix4 xform = m4_scalar(1.0);
					xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
					// xform = m4_translate(xform, v3(itemSprite->image->width * -0.5, 0.0, 0));
					// xform = m4_scale(xform, v3(0.5, 0.5, 1.0));
					if (player->pos.y > e->pos.y) draw_image_xform(itemSprite->image, xform, itemSprite->size, COLOR_WHITE);
				}
				// do doors, items?
			}
		}

		{
			// :render hover text
			Entity* entity = world->activeEntity;
			Object* object = world->activeObject;
			if (entity != 0 && world->mouseActive) 
			{
				Sprite* sprite = getSprite(entity->spriteID);
				// Item* item = getItem(world.activeItem);
				
				{ 
					Vector4 color = v4(1, 1, 1, 0.2f);
					// make rect around sprite and highlight on mouse over
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, entity->pos);
					if (world->debugOn) { draw_circle(hotspot.min, range2f_size(hotspot), color); }
					// maybe draw the interact rad or something else that is causing the issue?

					// draw hover text -  TODO: do measure text and center
					Vector2 hoverTextPos = v2(entity->pos.x, hotspot.max.y + 5.0);
					hoverTextPos = centerTextToPos(entity->hoverText, font, fontHeight, textScaling, hoverTextPos);
					draw_text(font, entity->hoverText, fontHeight, hoverTextPos, textScaling, COLOR_GREEN);
				}
			}
			else if (object != 0 && world->mouseActive)
			{
				//hover text
			}
			else if (!world->mouseActive) 
			{
				world->playerText = STR("");
				world->currentCursor = c_click;
			}

			if (world->playerText.data > 0)
			{
				if (worldFrame.nowTime - world->textTimer > textDuration)
				{
					world->textTimer = 0;
					world->playerText = STR("");
				}
				if (world->textTimer > 0 && worldFrame.nowTime - world->textTimer  < textDuration)
				{
					set_screen_space();
					Vector2 dialogueBoxPos = centerTextToPos(world->playerText, font, fontHeight, textScaling, v2(200.0, 74.0));
					draw_text(font, world->playerText, fontHeight, dialogueBoxPos, textScaling, COLOR_WHITE);
					set_world_space();
				}
			}

		}
		// :UI
		{
			// draw_frame.projection = m4_make_orthographic_projection(0.0, 400.0, 0.0, 300.0, -1, 10);
			set_screen_space();
			draw_frame.camera_xform = m4_scalar(1.0);
			 // maybe use backbuffer?

			if (world->uxStateID == ux_inventory)
			{	
				audio_player_set_state(bgNoisePlayer, AUDIO_PLAYER_STATE_PLAYING);
				
				if (audio_player_get_current_progression_factor(bgNoisePlayer) > 0.99)
				{
					audio_player_transition_to_source(bgNoisePlayer, *getAudioSource(au_train), 0.5);
				}

				// audio_player_set_progression_factor(song_player, 0);

				
				{ // UIBOX
					Matrix4 xform = m4_scalar(1.0); // m4_make_scale(v3(1,1,1))?
					xform = m4_translate(xform, v3(35.0, 15.0, 0.0));
					// draw_rect_xform(xform, v2(invWidth, invHeight), v4(1.0, 1.0, 1.0, 0.15));
					Sprite* sprite = getSprite(s_ui_inventory); 
					draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
				}
				// :UI inventory
				float invStartPosX = 50.0; // inv screen position
				float invStartPosY = 22.0;
				float invSlotWidth = 32;
				float invSlotHeight = 32;
				float invSlotPadding = 4;
				float invSlots = 8; // slots along bottom of screen, or 5 by 6 grid?
				// Item* invPage0[invRenderSlots]; // Maybe add a resizeable array using the temp alloc?

				float invWidth = invSlots * (invSlotWidth + invSlotPadding) + (invSlotPadding * 2.0);
				float invHeight = invSlotHeight + (invSlotPadding * 2.0);

				// :render UI loop over inv
				// if inventory currently visible?
				int invItemCount = 0;
				for (int i = 0; i < i_MAX; i++)
				{
					Item* item = world->currentInv[i];
					if (item != null) // && if dragged add alpha, and if successfully dropped delete
					{
						// invPage0[invItemCount] = item; // if in inv put in new array for rendering?
						invItemCount += 1;
						Matrix4 xform = m4_scalar(1.0);
						xform = m4_translate(xform, v3(invStartPosX + invSlotPadding, invStartPosY + invSlotPadding, 0.0));
						// draw_rect_xform(xform, v2(invSlotWidth, invSlotHeight), v4(1.0, 1.0, 1.0, 0.25));
						draw_image_xform(item->image, xform, item->size, COLOR_WHITE);

						{ // check mouse in item box - pull out to function
							Vector2 mousePosUI = getMouseCurrentProj(); // get Mouse in UI Coords
							Vector4 color = v4(1, 1, 1, 0.15f);
							// make rect around sprite and highlight on mouse over
							Range2f hotspot = getHotSpot(v2(invSlotWidth, invSlotHeight), v2(-invSlotPadding, -invSlotPadding));
							hotspot = range2f_shift(hotspot, v2(invStartPosX, invStartPosY));
							if (range2f_contains(hotspot, mousePosUI)) 
							{
								draw_rect(hotspot.min, range2f_size(hotspot), color);// highlight box
								float adjustScale = 0.5 * sinBob(time, 5.0); // maybe make an animation instead of sinBob
								xform = m4_scale(xform, v3(1.0, adjustScale, 1.0));
								world->activeItem = item;

								Vector2 pos = centerTextToPos(item->name, font, fontHeight, textScaling, v2(hotspot.min.x + (invSlotWidth / 2.0), hotspot.min.y - 20.0));
								draw_text(font, item->name, fontHeight, pos, textScaling, COLOR_WHITE);

								// invItemClicked(item, entity? )
							}
						}
						invStartPosX += invSlotWidth + invSlotPadding;

						// TODO: items are not added in pickup order for some reason  - NB its because it is in the inv struct order. how to fix this?
					}
				}

			}
			else if (world->uxStateID == ux_dialog)
			{	
				{ // UIBOX
					Matrix4 xform = m4_scalar(1.0); // m4_make_scale(v3(1,1,1))?
					xform = m4_translate(xform, v3(35.0, 10.0, 0.0));
					// draw_rect_xform(xform, v2(invWidth, invHeight), v4(1.0, 1.0, 1.0, 0.15));
					Sprite* sprite = getSprite(s_ui_dialog); 
					draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
				}

				// Stop player from clicking on anything but text
				world->currentCursor = c_click;
				if (range2f_contains(world->dialogueBox, worldFrame.mousePosScreen)) world->currentCursor = c_hot;

				Sprite* actorLSprite = getSprite(s_po_player);
				Sprite* actorRSprite = getSprite(world->actorR->portraitID);

				Vector2 dlgBoxPos = v2(35.0, 10.0);
				Vector2 dlgBoxSize = v2(330.0, 64.0);
				Vector2 textBoxPos = v2(99.0, 16.0);
				Vector2 textBoxSize = v2(260.0, 52.0);
				// draw_rect(dlgBoxPos, dlgBoxSize, v4(1.0, 1.0, 1.0, 0.15));
				// draw_rect(textBoxPos, textBoxSize, v4(1.0, 1.0, 1.0, 0.30));
				//string long_text = STR("Jaunty jackrabbits juggle quaint quilts \nand quirky quinces, quickly queuing \nup for a jubilant, jazzy jamboree \nin the jungle. CLICK ME");

				Dialog* d = getDialog(world->dialogID);
				world->dialog = d; // maybe need a better way to collect the dialog than a for loop.
				world->actorR->dialogFlag = d->flag;

				if (d->dialogID > 1000)
				{
					draw_image(actorLSprite->image, dlgBoxPos, actorLSprite->size, COLOR_WHITE);
					draw_text(font, d->text, fontHeight, v2(textBoxPos.x, textBoxPos.y + 40), textScaling, COLOR_WHITE);
					world->actorR->nextDialogID = d->nextDialogID;
					
				}
				else if (d->dialogID >= 200 && d->dialogID < 1000)
				{
					draw_image(actorRSprite->image, v2(dlgBoxPos.x + 266.0, dlgBoxPos.y), actorRSprite->size, COLOR_WHITE);
					draw_text(font, d->text, fontHeight, v2(textBoxPos.x - 50, textBoxPos.y + 40), textScaling, COLOR_WHITE);
					world->actorR->nextDialogID = d->nextDialogID;
					world->actorR->lastLineID = d->dialogID;
				}
				else if (d->dialogID == 0)
				{
					
					world->actorR = 0;
					world->dialog = 0;
					world->uxStateID = ux_inventory;
				}

			}
			else if (world->uxStateID == ux_menu)
			{	

				if (world->currentSongID != au_song1) 
				{
					audio_player_transition_to_source(songPlayer, *getAudioSource(au_song1), 1.0);
					audio_player_set_state(songPlayer, AUDIO_PLAYER_STATE_PLAYING);
				}
				audio_player_set_state(bgNoisePlayer, AUDIO_PLAYER_STATE_PAUSED);

				// Background Color
				draw_rect(v2(0.0, 0.0), v2(window.width, window.height), COLOR_BLACK);
				Vector2 centerTop = v2(200.0, 240.0);
				Vector2 pos = centerTop;
				float32 padding = 60.0;
				string text = STR("");
				Vector4 color = COLOR_WHITE;
				Object* objA = worldFrame.activeObject;

				// Game Title
				text = STR("Epic Game Title");
				pos = centerTextToPos(text, font, fontHeight, textScalingBig, pos);
				draw_text(font, text, fontHeight, pos, textScalingBig, color);
				
				// New Game
				text = STR("New Game"); // or Resume Game
				pos = v2(centerTop.x, centerTop.y - padding);
				pos = centerTextToPos(text, font, fontHeight, textScaling, pos);
				if (objA != null && objA->type == ot_newgame) color = COLOR_RED;
				draw_text(font, text, fontHeight, pos, textScaling, color);
				color = COLOR_WHITE;
				
				// Settings
				text = STR("Settings");
				pos = v2(centerTop.x, centerTop.y - (padding * 1.5));
				pos = centerTextToPos(text, font, fontHeight, textScaling, pos);
				if (objA != null && objA->type == ot_settings) color = COLOR_RED;
				draw_text(font, text, fontHeight, pos, textScaling, color);
				color = COLOR_WHITE;
				
				// Quit
				text = STR("Quit Game");\
				pos = v2(centerTop.x, centerTop.y - (padding * 2.0));
				pos = centerTextToPos(text, font, fontHeight, textScaling, pos);
				if (objA != null && objA->type == ot_quit) color = COLOR_RED;
				draw_text(font, text, fontHeight, pos, textScaling, color);
				color = COLOR_WHITE;
				
				// Credits
				text = STR("Made by Cuplinks and Dangergoose"); 
				pos = v2(centerTop.x, centerTop.y - (padding * 3.8));
				pos = centerTextToPos(text, font, fontHeight, textScalingSml, pos);
				draw_text(font, text, fontHeight, pos, textScalingSml, color);
			
			}
			else if (world->uxStateID == ux_settings)
			{
				// draw_rect(v2(0.0, 0.0), v2(window.width, window.height), COLOR_BLACK);
					
				// 	switch (screenSize)
				// 	{
				// 		case 2:
				// 			window.point_width = 400;
				// 			window.point_height = 300;
				// 			float zoom = 2.0;
				// 			break;
				// 		case 3:
				// 			window.point_width = 600;
				// 			window.point_height = 450;
				// 			float zoom = 3.0;
				// 			break;
				// 		case 4:
				// 			window.point_width = 800;
				// 			window.point_height = 600;
				// 			float zoom = 4.0;
				// 			break;
				// 	}

			}
			

			// :cursor
			{	
				if (world->isHWCursor)
				{
					// :hwCursor
					ShowCursor(true);
					Cursor* c = getCursor(world->currentCursor);
					os_set_mouse_pointer_custom(c->hwCursor);

					if (world->itemOnCursor) // && world->activeItem && world->activeItem->onCursor)
					{
						// ShowCursor(false);
						Vector2 mouseOffset = v2(worldFrame.mousePosScreen.x + 5.0, worldFrame.mousePosScreen.y - 19.0);
						draw_image(world->itemOnCursor->image, mouseOffset, v2_mulf(world->itemOnCursor->size, 0.5), COLOR_WHITE);
						world->itemOnCursor->inInventory = false;
						world->currentCursor = c_drag;
					}
					// else if (world->mouseActive == false) world->currentCursor = worldFrame.bg->hoverCursor;

				}
				else
				{
					ShowCursor(false);
					// :swCursor - not finished.
					// Vector2 mousePosUI = getMouseCurrentProj();
					// Matrix4 xform = m4_scalar(1.0);
					// xform = m4_translate(xform, v3(mousePosUI.x, mousePosUI.y, 0.0));
					// // draw_frame.projection = m4_make_orthographic_projection(0.0, window.width, 0.0, window.height, -1, 10);			
					// draw_image(cursor->image, mousePosUI, cursor->size, COLOR_RED);
					// //draw_rect(mousePosUI, v2(10, 10), COLOR_RED);
				}

			}

			// :fadeScreen
			if (world->screenFade.currentlyFadingOut) 
			{
				fadeOutScreen(&world->screenFade, 0.5f, COLOR_BLACK, worldFrame);
				// fadeOutAudio(world. currnet Auudio Source. volume do something....)
				draw_rect(v2(0.0, 0.0), v2(window.width, window.height), world->screenFade.color);
			} 
			if (world->screenFade.fadeAmount == 1.0f) 
			{	
				world->screenFade.currentlyFadingIn = true;
				// if (worldFrame.activeObject != null) // this happens if the mouse moves out of the active object - need to fix.
				{
					world->currentBG = world->warpBG;
					if (world->currentSongID != getAudioIDFromBG(world->warpBG))
					{
						world->currentSongID = getAudioIDFromBG(world->warpBG);
						audio_player_transition_to_source(songPlayer, *getAudioSource(world->currentSongID), 1.0);
					}
					
					// world->warpBG = 0; commenting this out seems to have fixed my black screen bug
					player->pos = world->warpPos;
					camera_pos.x = player->pos.x;
					world->uxStateID = ux_inventory;
				}
				
				// world->screenFade.startTime = worldFrame.nowTime;
			}
			if (world->screenFade.currentlyFadingIn)
			{
				fadeInScreen(&world->screenFade, 0.5f, worldFrame);
				// fade in audio....
				draw_rect(v2(0.0, 0.0), v2(window.width, window.height), world->screenFade.color);
			}


		} // end :UI

		{
			// :debug
			if (world->debugOn)
			{
				Vector2 mouseProjPos = getMouseCurrentProj();
				// draw_text(font, tprint("Mouse: %v2", v2(input_frame.mouse_x / 3.0, input_frame.mouse_y / 3.0)), fontHeight, v2(10, 10), v2(0.2, 0.2), COLOR_RED);
				draw_text(font, tprint("ScreenPos: [ %i, %i ]", (int)mouseProjPos.x, (int)mouseProjPos.y), fontHeight, v2(10, 290), textScalingSml, COLOR_RED);
				draw_text(font, tprint("WorldPos: [ %i, %i ]", (int)worldFrame.mousePosWorld.x, (int)worldFrame.mousePosWorld.y), fontHeight, v2(100, 290), textScalingSml, COLOR_RED);
				// draw_text(font, tprint("InputPos: [ %i, %i ]", (int)input_frame.mouse_x, (int)input_frame.mouse_y), fontHeight, v2(200, 10), textScalingSml, COLOR_RED);
				//draw_text(font, tprint("CameraPos: [ %i, %i ]", (int)camera_pos.x, (int)camera_pos.y), fontHeight, v2(300, 10), textScalingSml, COLOR_RED);

				// draw walkboxes and doors objects etc
				set_world_space();

				for (int i = 0; i < o_MAX; i++)
				{
					Object* obj = &world->objects[i];
					if (obj->objectID != o_nil)
					{
						drawQuadLines(obj->quad, 1.0, COLOR_GREEN);
						// if (isPointInConvexQuad(obj->quad, worldFrame.mousePosWorld)) 
						if (range2f_contains(range2f_make(obj->quad.q1, obj->quad.q3), worldFrame.mousePosWorld))
						{
							drawQuadLines(obj->quad, 1.0, COLOR_BLUE);
							// log("mouse in obj quad");
						}
					}
				}

				for (int i = 0; i < w_MAX; i++)
				{
					Walkbox* box = &world->walkboxes[i];
					drawBoxFromRange2f(box->box, 1.0, COLOR_RED);
				}

				// test line intersection

				set_screen_space();

			}
		}

		secCounter += worldFrame.deltaTime;
		frameCounter += 1;
		if (secCounter > 1.0f)
		{
			framerate = frameCounter;
			secCounter = 0.0f;
			frameCounter = 0;
		}
				if (world->debugOn)
		{
			draw_text(font, tprint("FPS: %i", framerate), fontHeight, v2(200, 290), textScalingSml, COLOR_RED);
		}

		
		os_update(); 		
		gfx_update();



	} // game loop

	return 0;
}

