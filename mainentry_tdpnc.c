
const int TILE_WIDTH = 16;

#include "src/utils.c"
#include "src/data.c"
#include "src/game.c"


int entry(int argc, char **argv) 
{
	// :init
	window.title = STR("TopDownPointAndClick");
	window.width = 640;
	window.height = 360;
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 600;
	window.y = 900;
	window.clear_color = hex_to_rgba(0x6495EDff);

	// Vector2 tileSize = v2((float)TILE_WIDTH, (float)TILE_WIDTH);

	Gfx_Font *font = load_font_from_disk(STR("assets/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading font");
	const u32 font_height = 10;

	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// implement a load room function for all entities
	// loadRoom(RoomID roomID);
	// and store this stuff elsewhere.
	loadSprite(s_rock0, STR("assets/rock0.png"), v2(0.0, 0.0), v2(0.0, 0.0));
	loadSprite(s_tree0, STR("assets/tree0.png"), v2(41.0, 30.0), v2(20.5, -15.0)); // test to see if can change clickable size
	loadSprite(s_player, STR("assets/player.png"), v2(0.0, 0.0), v2(0.0, 0.0));
	loadSprite(s_flower, STR("assets/flower0.png"), v2(0.0, 0.0), v2(0.0, 0.0));
	// loadSprite(s_inv_flower0, STR("assets/flower0.png"), v2(0.0, 0.0), v2(0.0, 0.0));
	loadInventoryItem(i_flower_pink, STR("Pink Flower"), STR("assets/flower0.png"), 0);
	Entity* rock0 = createEntity(t_rock, s_rock0, i_nil, v2(-40, 40), STR("Rock"), false, 0);
	Entity* tree0 = createEntity(t_tree, s_tree0, i_nil, v2(50, 40), null_string, false, 0);
	Entity* player = createEntity(t_player, s_player, i_nil, v2(0, 0), null_string, false, 0);
	Entity* flower0 = createEntity(t_flower, s_flower, i_flower_pink, v2(-30.0, -30.0), STR("Flower"), true, 0);

	// init timers and fps etc
	float64 time = os_get_current_time_in_seconds();
	float64 prevTime = time;
	float64 secCounter = 0.0;
	s32 frameCounter = 0.0;

	// :camera stuff - this is fugged with text rendering i think
	float64 cameraZoom = 5.0; // based on player sprite size 16*24
	Vector2 cameraPos = v2(0, 0); // center screen

	while (!window.should_close)
	{
		reset_temporary_storage();

		worldFrame = (WorldFrame){0};

		float64 now = os_get_current_time_in_seconds();
		float64 deltaTime = now - prevTime;
		prevTime = now;

		// :camera
		draw_frame.projection = m4_make_orthographic_projection(window.pixel_width * -0.5, 
																window.pixel_width * 0.5, 
																window.pixel_height * -0.5, 
																window.pixel_height * 0.5, -1, 10);
		Vector2 targetPos = player->pos;
		animateV2ToTarget(&cameraPos, targetPos, deltaTime, 5.0f);
		draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
		draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(cameraPos.x, cameraPos.y, 0)));
		draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0/cameraZoom, 1.0/cameraZoom, 1.0)));

		
		// Vector2 mouseTile = getTilePos(mousePosWorld, tileSize);
		// log("%i, %i", (int)mouseTile.x, (int)mouseTile.y);

		// :loop over inventory

		// for (int i = 0; i < i_MAX; i++)
		// {
		// 	Item* item = &world->inventory[i];
		// 	if (item->inInventory)
		// 	{

		// 	}
		// }

		//:input
		float playerSpeed = 100.0;
		Vector2 mousePosWorld = getMouseInWorldCoords();

		// :update loop over all entities
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid)
			{
				Sprite* sprite = getSprite(e->spriteID);

				// check mouse in entity box - pull out to function
				{
					Vector4 color = v4(1, 1, 1, 0.2f);
					// make rect around sprite and highlight on mouse over
					Range2f hotspot = getHotSpot(sprite->clickableSize, sprite->origin);
					hotspot = range2f_shift(hotspot, e->pos);
					if (range2f_contains(hotspot, mousePosWorld))
					{
						color.a = 1.0f;
						draw_rect(hotspot.min, range2f_size(hotspot), color);
						worldFrame.activeEntity = e;
					}
				}

			}	
		}

		// :mouse input/hover/click stuff
		{
			Entity* eSelected = worldFrame.activeEntity;
			if (worldFrame.activeEntity) 
			{
				draw_text(font, eSelected->hoverText, font_height, mousePosWorld, v2(1, 1), COLOR_BLACK);
			}
			// debug hover
			// if (worldFrame.activeEntity) { log("Hover True"); }
			// else { log("Hover False"); }

			if (is_key_just_pressed(MOUSE_BUTTON_LEFT))
			{
				consume_key_just_pressed(MOUSE_BUTTON_LEFT); // because ordering is important (UI v World clicks...)
				
				if (eSelected && eSelected->clickable)
				{	
					entityClicked(eSelected, player);
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
				// render cursor update?
				log("%i", player->verbState);
			}
			
		}

		// :keyboard input stuff
		{
		if (is_key_just_released(KEY_ESCAPE)) window.should_close = true;
		
		Vector2 input_axis = v2(0, 0);
		if (is_key_down('A')) input_axis.x -= 1.0;
		if (is_key_down('D')) input_axis.x += 1.0;
		if (is_key_down('S')) input_axis.y -= 1.0;
		if (is_key_down('W')) input_axis.y += 1.0;
		input_axis = v2_normalize(input_axis);

		player->pos = v2_add(player->pos, v2_mulf(input_axis, (playerSpeed * deltaTime)));
		}

		// drawing background tiles if using tiles...
		
		// Vector2 playerTile = getTilePos(player->pos, tileSize);
		// Vector2 tileRadius = v2(160, 90);
		// for (int x = (int)playerTile.x - (int)tileRadius.x; x < (int)playerTile.x + (int)tileRadius.x; x++)
		// {
		// 	for (int y = (int)playerTile.y - (int)tileRadius.y; y < (int)playerTile.y + (int)tileRadius.y; y++)
		// 	{
		// 		if ((x + (y % 2 == 0)) % 2 == 0)
		// 		{
		// 			Vector2 pos = v2_mul(v2(x, y), tileSize);
		// 			pos.x = pos.x + (tileSize.x * -0.5);
		// 			pos.y = pos.y + (tileSize.y * -0.5);
		// 			draw_rect(pos, tileSize, v4(0.1, 0.1, 0.1, 0.1));
		// 		}
		// 	}

		// 	// // draw active tile as rect
		// 	// Vector2 activeTile = getWorldPos(mouseTile, tileSize);
		// 	// activeTile.x = activeTile.x + (tileSize.x * -0.5);
		// 	// activeTile.y = activeTile.y + (tileSize.y * -0.5);
		// 	// draw_rect(activeTile, tileSize, v4(0.5, 0.5, 0.5, 0.5));
		// }



		// :render loop over entities - pull out to function - z indexing??
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid)
			{
				switch (e->type)
				{
				//case /* constant-expression */:
					/* code */
					//break;
				
				default:
					Sprite* sprite = getSprite(e->spriteID);
					Matrix4 xform = m4_scalar(1.0);
					if (e->clickable) xform = m4_translate(xform, v3(0, 1.5 * sinBreathe(time, 3.0), 0));
					xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
					xform = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));  
					// TOCHECK: why? move half of size.x? leftover from tiles? // to make pos centered?
					Vector4 color = COLOR_WHITE;
					draw_image_xform(sprite->image, xform, sprite->size, color);
					break;
				}
			}
		}

		// :render UI loop over inv - (draw inventory like Lorelei and the LASEREYES! ie open purse/bag beside player with a nice animation)
		{
			draw_frame.view = m4_scalar(1.0);
			draw_frame.projection = m4_make_orthographic_projection(0.0, 640.0, 0.0, 320.0, -1, 10);
			float xPos = 80.0; // inv screen position
			float yPos = 30.0;

			// int renderableInvSize = 30; // 5 by 6 grid? 
			// Item* invPage0[renderableInvSize]; // Maybe add a resizeable array using the temp alloc?
			int invItemCount = 0;
			for (int i = 0; i < i_MAX; i++)
			{
				Item* item = &world->inventory[i];
				if (item->inInventory)
				{
					// invPage0[invItemCount] = item; // if in inv put in new array for rendering?
					invItemCount += 1;
					Matrix4 xform = m4_scalar(1.0);
					xform = m4_translate(xform, v3(xPos, yPos, 0.0));
					draw_rect_xform(xform, v2(16.0, 16.0), COLOR_BLACK);
					draw_image_xform(item->image, xform, item->size, COLOR_WHITE);
				}
			}					
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