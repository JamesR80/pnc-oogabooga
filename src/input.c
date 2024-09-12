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

 void handleInput(Entity* player, Entity* eSelected, float64 deltaTime)
 {

    if (is_key_just_released(KEY_ESCAPE)) window.should_close = true;


    // Need *player, *room??
    // Maybe just start with mouse PnC!

    if (is_key_just_pressed(MOUSE_BUTTON_LEFT))
    {	
        // Need to know if mouse is in UI or world space.
        consume_key_just_pressed(MOUSE_BUTTON_LEFT); // because ordering is important (UI v World clicks...)
        
        if (eSelected && eSelected->clickable)
        {	
            entityClicked(eSelected, player);
            movePlayerToObject(player, eSelected, worldFrame);
        }
        else 
        {
            movePlayerToClick(player, worldFrame);
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

                break;

            case v_look:

                break;
            case v_grab:
                break;

            case v_use:
                break;

            default:
                break;
        }
        // render cursor update? text tooltip on mouse hover?
        log("%i", player->verbState);
    }
    
    // :keyboard input stuff
    {
    Vector2 input_axis = v2(0, 0);
    if (is_key_down('A')) input_axis.x -= 1.0;
    if (is_key_down('D')) input_axis.x += 1.0;
    if (is_key_down('S')) input_axis.y -= 1.0;
    if (is_key_down('W')) input_axis.y += 1.0;
    input_axis = v2_normalize(input_axis);

    player->pos = v2_add(player->pos, v2_mulf(input_axis, (player->speed * deltaTime)));
    }

 }
