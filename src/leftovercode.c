
			// {	// randy UX code - There has gotta be a better way to do this - move to input?
			// 	if (is_key_just_pressed(KEY_TAB)) // inv open key
			// 	{
			// 		consume_key_just_pressed(KEY_TAB);
			// 		world->uxStateID = (world->uxStateID == ux_inventory ? ux_nil : ux_inventory); // change state
			// 	}
			// 	world->inventoryAlphaTarget = (world->uxStateID == ux_inventory ? 1.0 : 0.0);
			// 	// animate to target?
			// 	animateF32ToTarget(&world->inventoryAlpha, world->inventoryAlphaTarget, deltaTime, 15.0f); // no animation yet
			// 	if (world->inventoryAlpha != 0.0) { }	// render the inv UI
			// 	bool isInvEnabled = world->inventoryAlphaTarget == 1.0; // is true if IAT is 1?

			// 	if (world->inventoryAlphaTarget != 0.0) {} // use just InvAlpha for animation, IAT is binary.
			// } // oh I get it, the invAlpha is actually an alpha value 0-1 that fades in and out the inv... 



            	// Gfx_Text_Metrics textMetrics = measure_text(font, eSelected->hoverText, fontHeight, v2(1, 1));
				// Vector2 justifyText = v2_sub(mousePosWorld, textMetrics.functional_pos_min);
				// justifyText = v2_sub(justifyText, v2_divf(textMetrics.functional_size, 2));
				// draw_text(font, eSelected->hoverText, fontHeight, mousePosWorld, textScaling, COLOR_BLACK);
				// draw_text(font, STR("HoverTest"), fontHeight, mousePosWorld, textScaling, COLOR_BLACK);
				// why is my text so fucked, also could draw_text_xform to a specific box maybe?



    // deprecated
	// :camera stuff - this is fugged with text rendering i think
	// float64 cameraZoom = 4.57; // based on player sprite size 16*24?
	// Vector2 cameraPos = player->pos; 
	// Vector2 roomMin = v2(0, 0);
	// Vector2 roomMax = v2(700, 300);
	// Vector2 roomSize = v2(ceil(roomMax.x - roomMin.x), ceil(roomMax.y - roomMin.y));
	// log("Room Size: %v2", roomSize);



			// // scroll background
		// float32 bgScrollLimit = getSprite(background->spriteID)->image->width - 200;
		// if (background->isScrollable && player->pos.x >= 200.0 && player->pos.x <= bgScrollLimit)
		// {
		// 	background->scrollPos = v2(player->pos.x - 200.0, player->pos.x + 200.0);
		// 	// draw_frame.projection = m4_make_orthographic_projection(background->scrollPos.x, background->scrollPos.y, 0.0, 300.0, -1, 10);
		// 	// animateF32ToTarget(&(background->scrollPos.x), player->pos.x +, deltaTime, rate);
		// }





		// :renderUI


				// Sprite* sprite = getSprite(e->spriteID);
				// Matrix4 xform = m4_scalar(1.0);
				// if (e->clickable) { xform = m4_translate(xform, v3(0, 1.5 * sinBob(time, 3.0), 0)); } // maybe add && hotspots visible and do highlight
				// xform = m4_translate(xform, v3(e->pos.x, e->pos.y, 0));
				// xform = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));
				// TOCHECK: why? move half of size.x? leftover from tiles? // to make pos centered?
				// Vector4 color = COLOR_WHITE;





					// // lets try a dialogue box by the player/npc etc
					// Sprite* playerSprite = getSprite(player->spriteID); // this could be in a more general scope maybe
					// Vector2 v2FrameSize = v2(playerSprite->frameWidth, playerSprite->frameHeight);
					// //Vector2 playerPos = getUIPosFromWorldPos(player->pos);

					// Vector2 dialogueBoxPos = v2_add(player->pos, v2(0.0, playerSprite->frameHeight)); 
					// // if text then get_measure text box etc, else min size
					// Vector2 dialogueBoxSize = v2(70.0, 30.0);
					// // draw_rect(dialogueBoxPos, dialogueBoxSize, v4(1.0, 1.0, 1.0, 0.5));