
#include "src/utils.c"
#include "src/data.c"
#include "src/animate.c"
#include "src/game.c"
#include "src/input.c"


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
	loadSprite(s_player, STR("jamAssets/characters/Adaline-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), true, 4, 1);
	loadSprite(s_ch_conductor, STR("jamAssets/characters/Conductor-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), true, 4, 1);

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
	Entity* conductor = createEntity(t_npc, s_ch_conductor, i_nil, v2(500, 110), STR("Conductor"), true, 0);

	// :createItems
	Entity* coupon = createEntity(t_object, s_item_coupon, i_coupon, v2(300, 110), STR("VIP Coupon"), true, 0);
	Entity* drink = createEntity(t_item, s_item_drink, i_drink, v2(-100, 0), STR("Fancy Cocktail"), true, 0);
	Entity* headshot = createEntity(t_item, s_item_headshot, i_headshot, v2(-100, 0), STR("Headshot of Starlet"), true, 0);
	Entity* key = createEntity(t_item, s_item_key, i_key, v2(-100, 0), STR("Brass Key"), true, 0);

	// :createInventoryItems
	loadInventoryItem(i_coupon, STR("VIP Drink Coupon"), STR("jamAssets/objects/coupon.png"), false, 0); // load this when needed? How?
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

	while (!window.should_close)
	{
		reset_temporary_storage();

		worldFrame = (WorldFrame){0};  // worldFrame is reset everyframe

		float64 time = os_get_elapsed_seconds();
		worldFrame.nowTime = time;
		worldFrame.deltaTime = worldFrame.nowTime - prevTime;
		prevTime = worldFrame.nowTime;

		worldFrame.bg = background; // = world.current bg or something...

		// :camera - if I am doing a fixed camera I should change this to be like the UI proj maybe
		draw_frame.camera_xform = m4_scalar(1.0);
		draw_frame.projection = m4_make_orthographic_projection(worldFrame.bg->scrollPos.x, worldFrame.bg->scrollPos.y, 0.0, 300.0, -1, 10); 

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
				world->currentCursor = e->hoverCursor;

				// draw hover text
				// do measure text and center
				Vector2 hoverTextPos = v2(hotspot.min.x, hotspot.max.y);
				draw_text(font, e->hoverText, fontHeight, hoverTextPos, textScaling, COLOR_GREEN);
			}
		}

		// :renderUI


				// Sprite* sprite = getSprite(e->spriteID);
				// Matrix4 xform = m4_scalar(1.0);
				// if (e->clickable) { xform = m4_translate(xform, v3(0, 1.5 * sinBob(time, 3.0), 0)); } // maybe add && hotspots visible and do highlight
				// xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
				// xform = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));
				// TOCHECK: why? move half of size.x? leftover from tiles? // to make pos centered?
				// Vector4 color = COLOR_WHITE;



		// :UI
		{
			// :Dialogue box?
			// lets try a dialogue box by the player/npc etc
			Sprite* playerSprite = getSprite(player->spriteID); // this could be in a more general scope maybe
			Vector2 v2FrameSize = v2(playerSprite->frameWidth, playerSprite->frameHeight);
			//Vector2 playerPos = getUIPosFromWorldPos(player->pos);

			Vector2 dialogueBoxPos = v2_add(player->pos, v2(0.0, playerSprite->frameHeight)); 
			// if text then get_measure text box etc, else min size
			Vector2 dialogueBoxSize = v2(70.0, 30.0);
			draw_rect(dialogueBoxPos, dialogueBoxSize, v4(1.0, 1.0, 1.0, 0.5));

			if (world->activeEntity && world->activeEntity->justClicked)
			{
				// TODO : get appropriate text - need a better data structure
				draw_text(font, world->activeEntity->lookText, fontHeight, dialogueBoxPos, textScaling, COLOR_WHITE);
				// need to fix delay !!! actually maybe don;t need a delay just on exit hopspot.
				// need to have an on exit hotSpot. need a callback system.
				// also the dialogue box text could be displayed based on measure text length
				
			}


			draw_frame.projection = m4_make_orthographic_projection(0.0, 400.0, 0.0, 300.0, -1, 10);
			// draw_frame.view = m4_scalar(1.0);

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
					// xform = m4_scale(xform, v3(2.0, 2.0, 1.0)); // this is just a hack, inv items should be in the right dimensions
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