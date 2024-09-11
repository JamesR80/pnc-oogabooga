
#include "src/utils.c"
#include "src/data.c"
#include "src/animate.c"
#include "src/input.c"
#include "src/game.c"


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

	Gfx_Font *font = load_font_from_disk(STR("assets/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading font");
	const u32 fontHeight = 48;
	render_atlas_if_not_yet_rendered(font, fontHeight, 'A'); 

	window.point_width = 600;
	window.point_height = 450; 

	// below is the window offest from bottom left of screen - maybe get screen dimensions and center it.
	window.point_x = 200;
	window.point_y = 200;
	window.clear_color = hex_to_rgba(0x6495EDff); // backgroung color

	// :cursor stuff
	Custom_Mouse_Pointer clickCursor 
		= os_make_custom_mouse_pointer_from_file(STR("jamAssets/cursors/select.png"), 24, 24, get_heap_allocator());
	assert(clickCursor != 0, "Could not load pointer");
	Custom_Mouse_Pointer lookCursor 
		= os_make_custom_mouse_pointer_from_file(STR("jamAssets/cursors/inspect.png"), 24, 24, get_heap_allocator());
	assert(lookCursor != 0, "Could not load pointer");
	Custom_Mouse_Pointer grabCursor 
		= os_make_custom_mouse_pointer_from_file(STR("jamAssets/cursors/pickup.png"), 24, 24, get_heap_allocator());
	assert(grabCursor != 0, "Could not load pointer");
	Custom_Mouse_Pointer useCursor 
		= os_make_custom_mouse_pointer_from_file(STR("jamAssets/cursors/activity.png"), 24, 24, get_heap_allocator());
	assert(useCursor != 0, "Could not load pointer");

	os_set_mouse_pointer_custom(clickCursor);


	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// implement a load room function for all entities
	// loadRoom(RoomID roomID);
	// and store this stuff elsewhere.
	// need a good way to draw a room and translate the positions of everything to the game.
	loadSprite(s_player, STR("jamAssets/characters/Adaline-Idle-Sheet.png"), v2(0.0, 0.0), v2(0.0, 0.0), 4, 1);

	// should be load background?
	loadSprite(s_bg_dining, STR("jamAssets/rooms/DiningCarBG.png"), v2(0.0, 0.0), v2(0.0, 0.0), 1, 1);

	Entity* background = createEntity(t_background, s_bg_dining, i_nil, v2(0, 0), null_string, false, 0);

	Entity* player = createEntity(t_player, s_player, i_nil, v2(175, 110), null_string, false, 0);
	// Entity* flower0 = createEntity(t_flower, s_flower0, i_flower_pink, v2(290, 150), STR("Pink Flower"), true, 0);

	// loadInventoryItem(i_flower_pink, STR("Pink Flower"), STR("assets/flower0.png"), 0); // load this when needed? How?


	// init timers and fps etc
	float64 prevTime = os_get_elapsed_seconds();
	float64 secCounter = 0.0;
	s32 frameCounter = 0.0;
	int delayCounter = 0; // maybe add a data struct to keep a bunch of timers

	
	// :camera stuff - this is fugged with text rendering i think
	// float64 cameraZoom = 4.57; // based on player sprite size 16*24?

	Vector2 cameraPos = player->pos; 

	Vector2 roomMin = v2(0, 0);
	Vector2 roomMax = v2(700, 300);
	Vector2 roomSize = v2(ceil(roomMax.x - roomMin.x), ceil(roomMax.y - roomMin.y));

	log("Room Size: %v2", roomSize);


	while (!window.should_close)
	{
		reset_temporary_storage();

		worldFrame = (WorldFrame){0};

		float64 time = os_get_elapsed_seconds();
		float64 nowTime = time;
		float64 deltaTime = nowTime - prevTime;
		prevTime = nowTime;

		// :camera - if I am doing a fixed camera I should change this to be like the UI proj maybe
		draw_frame.camera_xform = m4_scalar(1.0);
		// draw_frame.projection = m4_make_orthographic_projection(-roomSize.x/2, roomSize.x/2, -roomSize.y/2, roomSize.y/2, -1, 10);

		draw_frame.projection = m4_make_orthographic_projection(background->scrollPos.x, background->scrollPos.y, 0.0, 300.0, -1, 10); 

		
		// draw_frame.camera_xform = m4_make_translation(v3(v2_expand(cameraPos), 0.0));
		// draw_frame.camera_xform = m4_mul(draw_frame.camera_xform, m4_make_translation(v3(cameraPos.x, cameraPos.y, 0)));


		// draw_frame.camera_xform = m4_make_translation(v3(v2_expand(cameraPos), 0.0));
		// draw_frame.camera_xform = m4_mul(draw_frame.camera_xform, m4_make_scale(v3(1.0/2.0, 1.0/2.0, 1.0)));


		Vector2 textScaling = v2(0.5, 0.5);

		//:input
		player->speed = 100.0;
		Vector2 mousePosWorld = getMouseCurrentProj(); 

		// :update loop over all entities
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid)
			{
				Sprite* sprite = getSprite(e->spriteID);
				Item* item = getItem(e->itemID);

				// if (e->type == t_player) log("x: %f, y:%f", e->pos.x, e->pos.y);

				
				{ // check mouse in entity box - pull out to function
					Vector4 color = v4(1, 1, 1, 0.2f);
					// make rect around sprite and highlight on mouse over
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					// Maybe draw_circle? for hotspot? 
					hotspot = range2f_shift(hotspot, e->pos);
					if (range2f_contains(hotspot, mousePosWorld))
					{
						color.a = 1.0f;
						// draw_rect(hotspot.min, range2f_size(hotspot), color); // should this be in render?
						// draw_circle(hotspot.min, range2f_size(hotspot), color);
						worldFrame.activeEntity = e; // can i send this to render?
					}
				}
			}	
		}
		// :update loop over inventory
		// for (int i = 0; i < i_MAX; i++)
		// {
		// 	Item* item = &world->inventory[i];
		// 	if (item->inInventory)
		// 	{

		// 	}
		// }


		// :mouse input/hover/click stuff
		{
			Entity* eSelected = worldFrame.activeEntity; // worldFrame is reset everyframe
			if (worldFrame.activeEntity) 
			{
				// Gfx_Text_Metrics textMetrics = measure_text(font, eSelected->hoverText, fontHeight, v2(1, 1));
				// Vector2 justifyText = v2_sub(mousePosWorld, textMetrics.functional_pos_min);
				// justifyText = v2_sub(justifyText, v2_divf(textMetrics.functional_size, 2));
				draw_text(font, eSelected->hoverText, fontHeight, mousePosWorld, textScaling, COLOR_BLACK);
				// why is my text so fucked, also could draw_text_xform to a specific box maybe?
			}
			// debug hover
			// if (worldFrame.activeEntity) { log("Hover True"); }
			// else { log("Hover False"); }

			if (is_key_just_pressed(MOUSE_BUTTON_LEFT))
			{	
				// Need to know if mouse is in UI or world space.
				consume_key_just_pressed(MOUSE_BUTTON_LEFT); // because ordering is important (UI v World clicks...)
				
				if (eSelected && eSelected->clickable)
				{	
					entityClicked(eSelected, player);
					movePlayerToObject(player, eSelected, background, nowTime, deltaTime);
				}
				// set moving flag and keep moving until within radius, then change flag.
				// Move algorithm - 
				// If clicking on Obj, get look/use interact Pos
				// if in world, Check x and or y in a walkbox
				// if both xy in walkbox move to xy
				// if x only or y only -> create new point at border of walkbox and walk
				// if none, create new point to closest walkbox
				// navigate walkboxes? or polygonal space.
				else 
				{
					movePlayerToClick(player, background, mousePosWorld, nowTime, deltaTime);
				}
			}
			
			if (is_key_just_pressed(MOUSE_BUTTON_RIGHT))
			{
				consume_key_just_pressed(MOUSE_BUTTON_RIGHT);
				
				if (player->verbState > 0)
				{	
					player->verbState += 1;
					if (player->verbState == v_MAX) player->verbState = 1;
				}

				// render cursor update? text tooltip on mouse hover?
				switch (player->verbState)
				{
					case v_nil:
						// assert error
						break;

					case v_click:
						os_set_mouse_pointer_custom(clickCursor);

						break;

					case v_look:
						os_set_mouse_pointer_custom(lookCursor);

						break;
					case v_grab:
						os_set_mouse_pointer_custom(grabCursor);
						break;

					case v_use:
						os_set_mouse_pointer_custom(useCursor);
						break;

					default:
						os_set_mouse_pointer_custom(clickCursor);
						break;
				}
				// render cursor update? text tooltip on mouse hover?
				log("%i", player->verbState);
			}
			
		}

		handleInput(player, deltaTime);
		movePlayer(player, background, nowTime, deltaTime);



		// :render loop over entities - pull out to function - z indexing??
		
		// maybe render background before projection?
		Sprite* bgSprite = getSprite(background->spriteID); 
		draw_image(bgSprite->image, background->pos, bgSprite->size, COLOR_WHITE);

		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid)
			{
				// Sprite* sprite = getSprite(e->spriteID);
				// Matrix4 xform = m4_scalar(1.0);
				// if (e->clickable) { xform = m4_translate(xform, v3(0, 1.5 * sinBob(time, 3.0), 0)); } // maybe add && hotspots visible and do highlight
				// xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
				// xform = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));
				// TOCHECK: why? move half of size.x? leftover from tiles? // to make pos centered?
				// Vector4 color = COLOR_WHITE;

				switch (e->type)
				{

					case t_player:
						animate(e, nowTime, deltaTime);
						break;
					
					default:
						break;

				}
			}
		}

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
				draw_text(font, worldFrame.activeEntity->lookText, fontHeight, dialogueBoxPos, textScaling, COLOR_WHITE);\
				// need to fix delay !!! actually maybe don;t need a delay just on exit hopspot.
				// need to have an on exit hotSpot. need a callback system.
				delayCounter += 1;
				if (delayCounter >= 120) // 120 frames or 2 seconds at 60 fps - need to adjust this to frames.
				{
					worldFrame.activeEntity->justClicked = false;
					delayCounter = 0;
				}
				
			}


			draw_frame.projection = m4_make_orthographic_projection(0.0, 400.0, 0.0, 300.0, -1, 10);
			// draw_frame.view = m4_scalar(1.0);

			{	// randy UX code - There has gotta be a better way to do this - move to input?
				if (is_key_just_pressed(KEY_TAB)) // inv open key
				{
					consume_key_just_pressed(KEY_TAB);
					world->uxState = (world->uxState == ux_inventory ? ux_nil : ux_inventory); // change state
				}
				world->inventoryAlphaTarget = (world->uxState == ux_inventory ? 1.0 : 0.0);
				// animate to target?
				animateF32ToTarget(&world->inventoryAlpha, world->inventoryAlphaTarget, deltaTime, 15.0f); // no animation yet
				if (world->inventoryAlpha != 0.0) { }	// render the inv UI
				bool isInvEnabled = world->inventoryAlphaTarget == 1.0; // is true if IAT is 1?

				if (world->inventoryAlphaTarget != 0.0) {} // use just InvAlpha for animation, IAT is binary.
			} // oh I get it, the invAlpha is actually an alpha value 0-1 that fades in and out the inv... 

			
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
						}
					}
					draw_image_xform(item->image, xform, item->size, COLOR_WHITE);

					invStartPosX += invSlotWidth + invSlotPadding;

					// TODO: items are not added in pickup order for some reason
				}
			}



		} // end :UI

		{
			// :debug
			// draw_text(font, tprint("Mouse: %v2", v2(input_frame.mouse_x / 3.0, input_frame.mouse_y / 3.0)), fontHeight, v2(10, 10), v2(0.2, 0.2), COLOR_RED);


		}
		

		os_update(); 		
		gfx_update();

		secCounter += deltaTime;
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