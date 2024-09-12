
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

	Gfx_Font *font = load_font_from_disk(STR("jamAssets/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading font");
	const u32 fontHeight = 48;
	render_atlas_if_not_yet_rendered(font, fontHeight, 'A'); 

	window.point_width = 600;
	window.point_height = 450; 

	// below is the window offest from bottom left of screen - maybe get screen dimensions and center it.
	window.point_x = 200;
	window.point_y = 200;
	window.clear_color = hex_to_rgba(0x6495EDff); // backgroung color

	// disable windows cursor
	// ShowCursor(false);

	// :Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));
	

	// :load & create the stuff

	// implement a load room function for all entities and store this stuff elsewhere.
	// loadRoom(RoomID roomID);

	loadSprite(s_player, STR("jamAssets/characters/Adaline-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), 4, 1);
	loadSprite(s_ch_conductor, STR("jamAssets/characters/Conductor-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), 4, 1);

	// should be load background?
	loadSprite(s_bg_dining, STR("jamAssets/rooms/DiningCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), 1, 1);

	Entity* background = createEntity(t_background, s_bg_dining, i_nil, v2(0, 0), null_string, false, 0);
	Entity* player = createEntity(t_player, s_player, i_nil, v2(175, 110), null_string, false, 0);
	Entity* conductor = createEntity(t_npc, s_ch_conductor, i_nil, v2(500, 110), STR("Conductor"), true, 0);

	// Entity* flower0 = createEntity(t_flower, s_flower0, i_flower_pink, v2(290, 150), STR("Pink Flower"), true, 0);
	// loadInventoryItem(i_flower_pink, STR("Pink Flower"), STR("assets/flower0.png"), 0); // load this when needed? How?

	// :init timers and fps etc
	float64 prevTime = os_get_elapsed_seconds();
	float64 secCounter = 0.0;
	s32 frameCounter = 0.0;
	int delayCounter = 0; // maybe add a data struct to keep a bunch of timers

	// :init game misc
	player->speed = 100.0;
	Vector2 textScaling = v2(0.2, 0.2);

	while (!window.should_close)
	{
		reset_temporary_storage();

		worldFrame = (WorldFrame){0};  // worldFrame is reset everyframe

		float64 time = os_get_elapsed_seconds();
		worldFrame.nowTime = time;
		worldFrame.deltaTime = worldFrame.nowTime - prevTime;
		prevTime = worldFrame.nowTime;

		worldFrame.activeCursor = getCursor(c_click);
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
			if (e->isValid)
			{
				Sprite* sprite = getSprite(e->spriteID);
				Item* item = getItem(e->itemID);
				
				{ // check mouse in entity box - pull out to function
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, e->pos);
					if (range2f_contains(hotspot, worldFrame.mousePosWorld))
					{
						worldFrame.activeEntity = e; // can i send this to render?
						worldFrame.onEntity = true;
					}
				}
			}	
		}



		// :mouse input/hover/click stuff ( func inside the above?)
		// need to redo this to enter/exit hotspot I think, although it works for inventory for some reason.

		handleInput(player, worldFrame.activeEntity, worldFrame.deltaTime);
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

				if (e->type == t_object || t_door)
				{
					// render entity
				}
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
		if (worldFrame.onEntity == true)
		{ 
			Entity* eSelected = worldFrame.activeEntity;  
			if (eSelected->type != t_background) 
			{
				Sprite* sprite = getSprite(worldFrame.activeEntity->spriteID);
				// Item* item = getItem(worldFrame.activeItem->itemID);
				
				{ 
					Vector4 color = v4(1, 1, 1, 0.2f);
					// make rect around sprite and highlight on mouse over
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, eSelected->pos);
					// color.a = 1.0f;
					draw_rect(hotspot.min, range2f_size(hotspot), color); // should this be in render?
					draw_circle(hotspot.min, range2f_size(hotspot), color);
				}
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

			if (worldFrame.activeEntity && worldFrame.activeEntity->justClicked)
			{
				// TODO : get appropriate text - need a better data structure
				draw_text(font, worldFrame.activeEntity->lookText, fontHeight, dialogueBoxPos, textScaling, COLOR_WHITE);
				// need to fix delay !!! actually maybe don;t need a delay just on exit hopspot.
				// need to have an on exit hotSpot. need a callback system.
				// also the dialogue box text could be displayed based on measure text length
				delayCounter += 1;
				if (delayCounter >= 120) // 120 frames or 2 seconds at 60 fps - need to adjust this to frames.
				{
					worldFrame.activeEntity->justClicked = false;
					delayCounter = 0;
				}
				
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
					xform = m4_scale(xform, v3(2.0, 2.0, 1.0)); // this is just a hack, inv items should be in the right dimensions

					{ // check mouse in item box - pull out to function
						Vector2 mousePosUI = getMouseCurrentProj(); // get Mouse in UI Coords
						Vector4 color = v4(1, 1, 1, 0.35f);
						// make rect around sprite and highlight on mouse over
						Range2f hotspot = getHotSpot(v2(invSlotWidth, invSlotHeight), v2(-invSlotPadding, -invSlotPadding));
						hotspot = range2f_shift(hotspot, v2(invStartPosX, invStartPosY));
						if (range2f_contains(hotspot, mousePosUI)) 
						{
							draw_rect(hotspot.min, range2f_size(hotspot), color); // highlight box
							// float adjustScale = 0.5 * sinBob(time, 5.0); // maybe make an animation instead of sinBob
							xform = m4_scale(xform, v3(1.5, 1.5, 1.5));
							worldFrame.activeItem = item;
							draw_text(font, item->name, fontHeight, mousePosUI, textScaling, COLOR_BLACK);
							// draw_text(font, STR("HoverTest"), fontHeight, mousePosUI, textScaling, COLOR_BLACK);
						}
					}
					draw_image_xform(item->image, xform, item->size, COLOR_WHITE);

					invStartPosX += invSlotWidth + invSlotPadding;

					// TODO: items are not added in pickup order for some reason
				}
			}

			// :cursor
			{	
				Vector2 mousePosUI = getMouseCurrentProj();
				Matrix4 xform = m4_scalar(1.0);
				xform = m4_translate(xform, v3(mousePosUI.x, mousePosUI.y, 0.0));
				// draw_frame.projection = m4_make_orthographic_projection(0.0, window.width, 0.0, window.height, -1, 10);			
				Cursor* cursor =worldFrame.activeCursor;
				draw_image_xform(cursor->image, xform, cursor->size, COLOR_WHITE);
			}

		} // end :UI

		{
			// :debug
			Vector2 mouseProjPos = getMouseCurrentProj();
			// draw_text(font, tprint("Mouse: %v2", v2(input_frame.mouse_x / 3.0, input_frame.mouse_y / 3.0)), fontHeight, v2(10, 10), v2(0.2, 0.2), COLOR_RED);
			draw_text(font, tprint("Mouse: %v2", v2(mouseProjPos.x, mouseProjPos.y)), fontHeight, v2(10, 10), v2(0.2, 0.2), COLOR_RED);


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

	}

	return 0;
}