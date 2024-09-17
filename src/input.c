// TODO
// Input Options 
/*
1.  Two Button Mouse - standard PC Config
    Left Click Use, Right Click Look ala Secret Files

2.  One Button Mouse - MacOS?
    Need ALT button? OR Hold down?

3.  Stylus - iPad / Tablets etc 
    Need ALT button? OR Hold down?
    Or Maybe a gesture, circle item with stylus etc...

4.  Gamepad / WASD - direct control
    Use XY XO ER etc for USE/LOOK, hotspot activates if in range. Cycle through hotspots LR?


    OR
    https://www.youtube.com/watch?v=FC5ppRAsLl4
    Single-Click-Context-Sensitive
    OR
    Broken Sword - Two-Click-Context-Sensitive
*/

 void handleInput(Entity* player, Entity* activeEntity, Item* activeItem, float64 deltaTime)
 {

    if (is_key_just_released(KEY_ESCAPE)) window.should_close = true;

    if (is_key_just_released(KEY_TAB)) 
    {
        world->debugOn = !world->debugOn;
    }
    // Need *player, *room??
    // Maybe just start with mouse PnC!

    if (is_key_just_pressed(MOUSE_BUTTON_LEFT))
    {	
        // Need to know if mouse is in UI or world space.
        consume_key_just_pressed(MOUSE_BUTTON_LEFT); // because ordering is important (UI v World clicks...)

        if (world->debugOn) log("v2(%i, %i)", (int)worldFrame.mousePosWorld.x, (int)worldFrame.mousePosWorld.y);

        if (world->uxStateID == ux_dialog)
        {
            if (range2f_contains(world->dialogueBox, worldFrame.mousePosScreen))
            {
                // draw_rect(world->dialogueBox.min, world->dialogueBox.max, COLOR_RED);
                // skip to next dialog
                world->uxStateID = ux_inventory;
            }
        }
        else if (world->uxStateID == ux_inventory)
        {
            if (range2f_contains(world->gameBox, worldFrame.mousePosScreen))
            {
                if (activeEntity && activeEntity->clickable) // check if in interact rad
                {	
                    entityClicked(activeEntity, player, true);
                    movePlayerToObject(player, activeEntity->interactPos, worldFrame);
                    log("Moving to Object: %s", activeEntity->hoverText);
                }
                else
                {
                    movePlayerToClick(player, worldFrame);
                }
                if (worldFrame.activeObject->objectID > 0)
                {
                    objectClicked(worldFrame.activeObject, player, true);
                    movePlayerToObject(player, worldFrame.activeObject->interactPos, worldFrame);
                }
            }
            if (range2f_contains(world->dialogueBox, worldFrame.mousePosScreen))
            {
                if (activeItem)
                {
                    itemClicked(activeItem, true);
                    // add to cursor
                }
            }

        }
        else if (world->uxStateID == ux_menu || world->uxStateID == ux_nil)
        { }
    }
    
    if (is_key_just_pressed(MOUSE_BUTTON_RIGHT))
    {
        consume_key_just_pressed(MOUSE_BUTTON_RIGHT);
        
        // :examine
        if (activeEntity)
        {
            world->playerText = activeEntity->lookText;
            entityClicked(activeEntity, player, false);
        }
        else if (activeItem)
        {
            // world->playerText = activeItem->lookText;
            itemClicked(activeItem, false);
        }
        world->textBoxTime = worldFrame.nowTime;
        
    }
    
    // :keyboard input stuff
    // {
    // Vector2 input_axis = v2(0, 0);
    // if (is_key_down('A')) input_axis.x -= 1.0;
    // if (is_key_down('D')) input_axis.x += 1.0;
    // if (is_key_down('S')) input_axis.y -= 1.0;
    // if (is_key_down('W')) input_axis.y += 1.0;
    // input_axis = v2_normalize(input_axis);

    // player->pos = v2_add(player->pos, v2_mulf(input_axis, (player->speed * deltaTime)));
    // }

 }
