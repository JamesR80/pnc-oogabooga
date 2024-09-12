
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