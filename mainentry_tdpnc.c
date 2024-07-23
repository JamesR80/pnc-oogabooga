
const int TILE_WIDTH = 16;

#include "src/utils.c"
#include "src/data.c"


// Vector2 getMouseInTileCoords() {}


int entry(int argc, char **argv) 
{
	
	window.title = STR("TopDownPointAndClick");
	window.scaled_width = 1920; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 1080; 
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x6495EDff);

	Vector2 tileSize = v2((float)TILE_WIDTH, (float)TILE_WIDTH);

	Gfx_Font *font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf");
	
	const u32 font_height = 12;

	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// implement a load room function for all entities
	// loadRoom(RoomID roomID);
	// and store this stuf elsewhere.
	sprites[s_rock0] = (Sprite){ .image=load_image_from_disk(STR("assets/rock0.png"), get_heap_allocator()), .size=v2(14.0, 14.0)};
	sprites[s_tree0] = (Sprite){ .image=load_image_from_disk(STR("assets/tree0.png"), get_heap_allocator()), .size=v2(41.0, 70.0)};
	sprites[s_player] = (Sprite){ .image=load_image_from_disk(STR("assets/player.png"), get_heap_allocator()), .size=v2(15.0, 24.0)};
	//assert(sprites[s_player], "player.png failed to load"); ?

	Entity* rock0 = createEntity(t_rock, s_rock0, v2(-40, 40), tileSize);
	Entity* tree0 = createEntity(t_tree, s_tree0, v2(50, 40), tileSize);
	Entity* player = createEntity(t_player, s_player, v2(0, 0), tileSize);

	// fps
	float64 prevTime = os_get_current_time_in_seconds();
	float64 secCounter = 0.0;
	s32 frameCounter = 0.0;

	float64 cameraZoom = 4.57; // based on player sprite size 16*24
	Vector2 cameraPos = v2(0, 0); // center screen

	while (!window.should_close)
	{
		reset_temporary_storage();

		float64 now = os_get_current_time_in_seconds();
		float64 deltaTime = now - prevTime;
		prevTime = now;

		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		// :camera
		Vector2 targetPos = player->pos;
		animateV2ToTarget(&cameraPos, targetPos, deltaTime, 5.0f);
		draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
		draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(cameraPos.x, cameraPos.y, 0)));
		draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0/cameraZoom, 1.0/cameraZoom, 1.0)));

		Vector2 mousePosWorld = getMouseInWorldCoords();
		Vector2 mouseTile = getTilePos(mousePosWorld, tileSize);
		log("%i, %i", (int)mouseTile.x, (int)mouseTile.y);

		//:input
		// check mouse in entity box
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* e = &world->entities[i];
			if (e->isValid)
			{
				Sprite* sprite = getSprite(e->spriteID);

				

				/* // make rect around sprite and highlight on mouse over
				Range2f bounds = range2f_make_bottom_center(sprite->size);
				bounds = range2f_shift(bounds, e->pos);
				bounds.min = v2_sub(bounds.min, v2(5.0, 5.0));
				bounds.max = v2_add(bounds.max, v2(5.0, 5.0));
				Vector4 color = v4(1, 1, 1, 0.2f);
				if (range2f_contains(bounds, mousePosWorld)) color.a = 1.0f;
				draw_rect(bounds.min, range2f_size(bounds), color); 
				*/
			}	
		}

		if (is_key_just_released(KEY_ESCAPE)) window.should_close = true;
		
		Vector2 input_axis = v2(0, 0);
		if (is_key_down('A')) input_axis.x -= 1.0;
		if (is_key_down('D')) input_axis.x += 1.0;
		if (is_key_down('S')) input_axis.y -= 1.0;
		if (is_key_down('W')) input_axis.y += 1.0;
		input_axis = v2_normalize(input_axis);

		float playerSpeed = 100.0;
		player->pos = v2_add(player->pos, v2_mulf(input_axis, (playerSpeed * deltaTime)));


		// drawing background tiles
		Vector2 playerTile = getTilePos(player->pos, tileSize);
		Vector2 tileRadius = v2(160, 90);
		for (int x = (int)playerTile.x - (int)tileRadius.x; x < (int)playerTile.x + (int)tileRadius.x; x++)
		{
			for (int y = (int)playerTile.y - (int)tileRadius.y; y < (int)playerTile.y + (int)tileRadius.y; y++)
			{
				if ((x + (y % 2 == 0)) % 2 == 0)
				{
					Vector2 pos = v2_mul(v2(x, y), tileSize);
					pos.x = pos.x + (tileSize.x * -0.5);
					pos.y = pos.y + (tileSize.y * -0.5);
					draw_rect(pos, tileSize, v4(0.1, 0.1, 0.1, 0.1));
				}
			}
			Vector2 activeTile = getWorldPos(mouseTile, tileSize);
			// draw active tile as rect
			// activeTile.x = activeTile.x + (tileSize.x * -0.5);
			// activeTile.y = activeTile.y + (tileSize.y * -0.5);
			// draw_rect(activeTile, tileSize, v4(0.5, 0.5, 0.5, 0.5));
		}

		// :render
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
					xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
					xform = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));
					draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
					// draw_text(font, STR("Hello"), font_height, e->pos, v2(1, 1), COLOR_WHITE);

					break;
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