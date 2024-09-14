
#include "src/utils.c"
#include "src/data.c"
#include "src/animate.c"
#include "src/game.c"
#include "src/input.c"
#include "src/init.c"


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
	window.allow_resize = false;

	Gfx_Font *font = load_font_from_disk(STR("jamAssets/fonts/NotJamOldStyle11.ttf"), get_heap_allocator());
	assert(font, "Failed loading font");
	const u32 fontHeight = 48;
	render_atlas_if_not_yet_rendered(font, fontHeight, 'A'); 

	window.point_width = 600;
	window.point_height = 450; 

	// below is the window offest from bottom left of screen - maybe get screen dimensions and center it.
	window.point_x = 300;
	window.point_y = 300;
	window.clear_color = hex_to_rgba(0x6495EDff); // backgroung color

	// disable windows cursor
	ShowCursor(false);

	// :Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));
	

	// :load & create the stuff
	// implement a load room function for all entities and store this stuff elsewhere. loadRoom(RoomID roomID);

	// :loadCursors
	world->isHWCursor = true;
	loadCursor(c_click, STR("jamAssets/cursors/click.png"));
	loadCursor(c_grab, STR("jamAssets/cursors/grab.png"));
	loadCursor(c_look, STR("jamAssets/cursors/look.png"));
	loadCursor(c_talk, STR("jamAssets/cursors/talk.png"));
	loadCursor(c_left, STR("jamAssets/cursors/left.png"));
	loadCursor(c_right, STR("jamAssets/cursors/right.png"));

	// :loadCharacters
	loadSprite(s_player, STR("jamAssets/characters/Adaline-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), true, 8, 5);
	loadSprite(s_ch_conductor, STR("jamAssets/characters/Conductor-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), true, 4, 1);
	loadSprite(s_ch_reporter, STR("jamAssets/characters/Reporter-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), true, 4, 1);

	// :loadPortraits
	loadSprite(s_po_player, STR("jamAssets/portraits/MCPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_baron, STR("jamAssets/portraits/BaronPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);
	loadSprite(s_po_reporter, STR("jamAssets/portraits/ReporterPortrait.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 1, 1);


	// :loadItems
	loadSprite(s_item_coupon, STR("jamAssets/objects/coupon.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_drink, STR("jamAssets/objects/drink.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_headshot, STR("jamAssets/objects/headshot.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);
	loadSprite(s_item_key, STR("jamAssets/objects/key.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);

	// :loadBackgrounds
	loadSprite(s_bg_dining, STR("jamAssets/rooms/DiningCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), false, 0, 0);

	// :createEntities and Objects
	Entity* background = createEntity(t_background, s_bg_dining, i_nil, v2(0, 0), null_string, false, 0);
	Entity* player = createEntity(t_player, s_player, i_nil, v2(175, 110), null_string, false, 0);
	Entity* conductor = createEntity(t_npc, s_ch_conductor, i_nil, v2(110, 110), STR("Conductor"), true, 0);
	Entity* reporter = createEntity(t_npc, s_ch_reporter, i_nil, v2(500, 110), STR("Reporter"), true, 0);

	// add this to createEntity() 
	conductor->interactPos.x = conductor->pos.x + 30.0; // + if facing right, - if facing left
	reporter->interactPos.x = reporter->pos.x - 30.0;


	// :createItems
	Entity* coupon = createEntity(t_object, s_item_coupon, i_coupon, v2(300, 110), STR("Coupon"), true, 0);
	Entity* drink = createEntity(t_item, s_item_drink, i_drink, v2(-100, 0), STR("Fancy Cocktail"), true, 0);
	Entity* headshot = createEntity(t_item, s_item_headshot, i_headshot, v2(-100, 0), STR("Headshot of Starlet"), true, 0);
	Entity* key = createEntity(t_item, s_item_key, i_key, v2(-100, 0), STR("Brass Key"), true, 0);

	// :createInventoryItems
	loadInventoryItem(i_coupon, STR("Drink Coupon"), STR("jamAssets/objects/coupon.png"), false, 0); // load this when needed? How?
	loadInventoryItem(i_key, STR("Brass Key"), STR("jamAssets/objects/key.png"), true, 0);

	// :init timers and fps etc
	float64 prevTime = os_get_elapsed_seconds();
	float64 secCounter = 0.0;
	s32 frameCounter = 0.0;
	int delayCounter = 0; // maybe add a data struct to keep a bunch of timers

	// :init game misc
	player->speed = 100.0;
	Vector2 textScaling = v2(0.2, 0.2);
	world->currentCursor = c_click;
	world->mouseActive = false;
	Matrix4 camera_xform = m4_scalar(1.0);
	Vector2 camera_pos = v2(200, 110);
	float zoom = 3.0;
	world->uxStateID = ux_inventory;
	world->playerText = STR("");
	world->dialogueBox = range2f_make(v2(30.0, 15.0), v2(370.0, 75.0));

	while (!window.should_close)
	{
		reset_temporary_storage();

		worldFrame = (WorldFrame){0};  // worldFrame is reset everyframe

		float64 time = os_get_elapsed_seconds();
		worldFrame.nowTime = time;
		float64 deltaTime = worldFrame.nowTime - prevTime;
		worldFrame.deltaTime = deltaTime;
		prevTime = worldFrame.nowTime;

		worldFrame.bg = background; // = world.current bg or something...
		worldFrame.player = player; 

		// :camera - 
		// draw_frame.projection = m4_make_orthographic_projection(worldFrame.bg->scrollPos.x, worldFrame.bg->scrollPos.y, 0.0, 300.0, -1, 10);
		// worldFrame.world_proj = m4_make_orthographic_projection(0.0, screenWidth, 0.0, screenHeight, -1, 10);

		worldFrame.world_proj = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);
		Vector2 target_pos = player->pos;
		smoothCam(&camera_pos, target_pos, deltaTime, 15.0f);

		worldFrame.world_view = m4_identity;
		// translate into position
		worldFrame.world_view = m4_translate(worldFrame.world_view, v3(camera_pos.x, 150.0, 0));

		// scale the zoom
		worldFrame.world_view = m4_scale(worldFrame.world_view, v3(1.0/zoom, 1.0/zoom, 1.0));
		
		set_world_space();

		// :world stuff
		worldFrame.mousePosWorld = getMouseCurrentProj();
		Vector2 mousePosInput = v2(input_frame.mouse_x, input_frame.mouse_y);


		// :update loop over inventory
		// for (int i = 0; i < i_MAX; i++)
		// {
		// 	Item* item = &world->inventory[i];
		// 	if (item->inInventory)
		// 	{

		// 	}
		// }

		// :update loop over all entities
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];

			// :mouseOver
			if (e->isValid)
			{

				Sprite* sprite = getSprite(e->spriteID);
				Item* item = getItem(e->itemID);
				
				// check mouse in entity box - pull out to function
				Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
				hotspot = range2f_shift(hotspot, e->pos);

				if (range2f_contains(hotspot, worldFrame.mousePosWorld))
				{
					world->activeEntity = e;
					world->mouseActive = true;
					world->currentCursor = e->hoverCursor;
				}
				if (e == background ) 
				{

					world->activeEntity = 0;
					world->mouseActive = false;
					world->currentCursor = e->hoverCursor;
				}
				if (world->activeEntity && (world->textBoxTime - worldFrame.nowTime < -3))
				{
					world->activeEntity->justClicked = false; // this is happening too quickly
					world->playerText = STR("");

				}
			}	
		}



		// :mouse input/hover/click stuff ( func inside the above?)
		// need to redo this to enter/exit hotspot I think, although it works for inventory for some reason.

		handleInput(player, world->activeEntity, worldFrame.deltaTime);
		movePlayer(player, background, worldFrame.nowTime, worldFrame.deltaTime);


		// :render loop over entities - pull out to function - z indexing??
		
		// :renderBackground
		Sprite* bgSprite = getSprite(background->spriteID); 
		draw_image(bgSprite->image, background->pos, bgSprite->size, COLOR_WHITE);

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

		{
			// :renderActiveEntity stuff
			Entity* e = world->activeEntity; 
			if (world->mouseActive && e->type != t_background) 
			{
				Sprite* sprite = getSprite(e->spriteID);
				// Item* item = getItem(worldFrame.activeItem->itemID);
				
				{ 
					Vector4 color = v4(1, 1, 1, 0.2f);
					// make rect around sprite and highlight on mouse over
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, e->pos);
					// draw_rect(hotspot.min, range2f_size(hotspot), color); // should this be in render?
					draw_circle(hotspot.min, range2f_size(hotspot), color);
					// maybe draw the interact rad or something else that is causing the issue?

					// draw hover text
					// do measure text and center
					Vector2 hoverTextPos = v2(hotspot.min.x, hotspot.max.y);
					draw_text(font, e->hoverText, fontHeight, hoverTextPos, textScaling, COLOR_GREEN);
				}
				if (world->activeEntity->justClicked)
				{
					// lets try a dialogue box by the player/npc etc
					Sprite* playerSprite = getSprite(player->spriteID); // this could be in a more general scope maybe
					Vector2 v2FrameSize = v2(playerSprite->frameWidth, playerSprite->frameHeight);
					//Vector2 playerPos = getUIPosFromWorldPos(player->pos);

					Vector2 dialogueBoxPos = v2_add(player->pos, v2(0.0, playerSprite->frameHeight)); 
					// if text then get_measure text box etc, else min size
					Vector2 dialogueBoxSize = v2(70.0, 30.0);
					// draw_rect(dialogueBoxPos, dialogueBoxSize, v4(1.0, 1.0, 1.0, 0.5));
					draw_text(font, world->playerText, fontHeight, dialogueBoxPos, textScaling, COLOR_BLACK);
				}
			}
			else if (world->mouseActive) world->currentCursor = background->hoverCursor;


			// :Dialogue box?


			
			{
				// TODO : get appropriate text - need a better data structure

				// get state from entity,
				// draw relative state text
				
				
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
				

				// :UI inventory
				float invStartPosX = 34.0; // inv screen position
				float invStartPosY = 30.0;
				float invSlotWidth = 32;
				float invSlotHeight = 32;
				float invSlotPadding = 4;
				float invSlots = 9; // slots along bottom of screen, or 5 by 6 grid?
				// Item* invPage0[invRenderSlots]; // Maybe add a resizeable array using the temp alloc?

				float invWidth = invSlots * (invSlotWidth + invSlotPadding) + (invSlotPadding * 2.0);
				float invHeight = invSlotHeight + (invSlotPadding * 2.0);

				{
					Matrix4 xform = m4_scalar(1.0); // m4_make_scale(v3(1,1,1))?
					xform = m4_translate(xform, v3(invStartPosX, invStartPosY, 0.0));
					draw_rect_xform(xform, v2(invWidth, invHeight), v4(1.0, 1.0, 1.0, 0.15));
				}
				// :render UI loop over inv
				// if inventory currently visible?
				int invItemCount = 0;
				for (int i = 0; i < i_MAX; i++)
				{
					Item* item = &world->inventory[i];
					if (item->inInventory)
					{
						// invPage0[invItemCount] = item; // if in inv put in new array for rendering?
						invItemCount += 1;
						Matrix4 xform = m4_scalar(1.0);
						xform = m4_translate(xform, v3(invStartPosX + invSlotPadding, invStartPosY + invSlotPadding, 0.0));
						draw_rect_xform(xform, v2(invSlotWidth, invSlotHeight), v4(1.0, 1.0, 1.0, 0.25));
						draw_image_xform(item->image, xform, item->size, COLOR_WHITE);

						{ // check mouse in item box - pull out to function
							Vector2 mousePosUI = getMouseCurrentProj(); // get Mouse in UI Coords
							Vector4 color = v4(1, 1, 1, 0.35f);
							// make rect around sprite and highlight on mouse over
							Range2f hotspot = getHotSpot(v2(invSlotWidth, invSlotHeight), v2(-invSlotPadding, -invSlotPadding));
							hotspot = range2f_shift(hotspot, v2(invStartPosX, invStartPosY));
							if (range2f_contains(hotspot, mousePosUI)) 
							{
								draw_rect(hotspot.min, range2f_size(hotspot), color); // highlight box
								float adjustScale = 0.5 * sinBob(time, 5.0); // maybe make an animation instead of sinBob
								// xform = m4_scale(xform, v3(1.5, 1.5, 1.5));
								worldFrame.activeItem = item;

								xform = m4_translate(xform, v3(1.0, invStartPosY + (invSlotPadding * 2.0), 0.0)); // need this measured and centered
								draw_text_xform(font, item->name, fontHeight, xform, textScaling, COLOR_BLUE);
								// draw_text(font, STR("HoverTest"), fontHeight, mousePosUI, textScaling, COLOR_BLACK);
							}
						}
						invStartPosX += invSlotWidth + invSlotPadding;

						// TODO: items are not added in pickup order for some reason
					}
				}

			}
			else if (world->uxStateID == ux_dialog)
			{	
				// Stop player from clicking on anything but text

				Sprite* actorLSprite = getSprite(s_po_player);
				Sprite* actorRSprite = getSprite(s_po_baron);

				Vector2 dlgBoxPos = v2(35.0, 15.0);
				Vector2 dlgBoxSize = v2(330.0, 65.0);
				Vector2 textBoxPos = v2(100.0, 20.0);
				Vector2 textBoxSize = v2(200.0, 55.0);
				draw_rect(dlgBoxPos, dlgBoxSize, v4(1.0, 1.0, 1.0, 0.15));
				draw_rect(textBoxPos, textBoxSize, v4(1.0, 1.0, 1.0, 0.30));

				draw_image(actorLSprite->image, dlgBoxPos, actorLSprite->size, COLOR_WHITE);
				draw_image(actorRSprite->image, v2(dlgBoxPos.x + 266.0, dlgBoxPos.y), actorRSprite->size, COLOR_WHITE);
				string text = STR("Lorem ipsum dolor sit amet");
				string long_text = STR("Jaunty jackrabbits juggle \nquaint quilts and quirky \nquinces, quickly queuing up \nfor a jubilant, jazzy jamboree \nin the jungle. CLICK ME");

				draw_text(font, long_text, fontHeight, v2(textBoxPos.x, textBoxPos.y + 45), textScaling, COLOR_WHITE);
								

			}
			else if (world->uxStateID == ux_menu)
			{}


			// :cursor
			{	
				if (world->isHWCursor)
				{
					// :hwCursor
					ShowCursor(true);
					Cursor* c = getCursor(world->currentCursor);
					os_set_mouse_pointer_custom(c->hwCursor);
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

		} // end :UI

		{
			// :debug
			Vector2 mouseProjPos = getMouseCurrentProj();
			// draw_text(font, tprint("Mouse: %v2", v2(input_frame.mouse_x / 3.0, input_frame.mouse_y / 3.0)), fontHeight, v2(10, 10), v2(0.2, 0.2), COLOR_RED);
			draw_text(font, tprint("ScreenPos: [ %i, %i ]", (int)mouseProjPos.x, (int)mouseProjPos.y), fontHeight, v2(10, 10), v2(0.1, 0.1), COLOR_RED);
			draw_text(font, tprint("WorldPos: [ %i, %i ]", (int)worldFrame.mousePosWorld.x, (int)worldFrame.mousePosWorld.y), fontHeight, v2(100, 10), v2(0.1, 0.1), COLOR_RED);
			draw_text(font, tprint("InputPos: [ %i, %i ]", (int)input_frame.mouse_x, (int)input_frame.mouse_y), fontHeight, v2(200, 10), v2(0.1, 0.1), COLOR_RED);
			draw_text(font, tprint("CameraPos: [ %i, %i ]", (int)camera_pos.x, (int)camera_pos.y), fontHeight, v2(300, 10), v2(0.1, 0.1), COLOR_RED);
			


		}
		

		os_update(); 		
		gfx_update();

		secCounter += worldFrame.deltaTime;
		frameCounter += 1;
		if (secCounter > 1.0f)
		{
			// log("fps: %i", frameCounter);
			secCounter = 0.0f;
			frameCounter = 0;
		}

	} // game loop

	return 0;
}