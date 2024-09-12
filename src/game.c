

void entityClicked(Entity* e, Entity* player) // entity clicked or just screen clicked?
{
	e->justClicked = true;
	log("click!");
	if (fabsf(v2_dist(e->pos, player->pos)) < e->interactRadius) 
	{
		e->interactable = true;
	}
	else
	{
		e->interactable = false;
		// move to entity...
	} 
 
	switch (player->verbState)
	{
		case v_nil:
			log("nothing!");
			break;

		case v_look:
			log("looking!");
			e->lookText = STR("I'm looking at this"); // I need to set these at loadRoom via DB i think
			break;

		case v_use:
			log("using!");
			
			// if entity can be picked up && player is within range
			if (e->interactable)
			{
				e->useText = STR("I'll just grab this for now");				
				world->inventory[e->itemID].inInventory = true;
				destroyEntity(e);
			}
			else e->useText = STR("I can't use it from here");
			// and set text flag...
			break;

		default:
			// run obj clicked script?
			// move player to click?

			break;
	}
	// some delay set justClicked to false.
}

// set moving flag and keep moving until within radius, then change flag.
// Move algorithm - 
// If clicking on Obj, get look/use interact Pos
// if in world, Check x and or y in a walkbox
// if both xy in walkbox move to xy
// if x only or y only -> create new point at border of walkbox and walk
// if none, create new point to closest walkbox
// navigate walkboxes? or polygonal space.

void movePlayer(Entity* player, Entity* background, float64 nowTime, float64 deltaTime)
{
	Vector2 direction = v2_sub(player->destPos, player->pos);
	float distance = v2_length(direction);

	if (distance > 1.0f && player->isMoving)
	{	
		direction = v2_divf(direction, distance);
		float movement = player->speed * deltaTime;
		player->pos.x += direction.x * movement;
		player->pos.y += direction.y * movement;

		// clamp movement to walkbox
		if (player->pos.x < 90.0) player->pos.x = 90.0;
		if (player->pos.y < 100.0) player->pos.y = 100.0;
		if (player->pos.x > 610.0) player->pos.x = 610.0;
		if (player->pos.y > 150.0) player->pos.y = 150.0;

		// scroll background
		if (background->isScrollable && player->pos.x >= 200.0 && player->pos.x <= 500.0)
		{
			background->scrollPos = v2(player->pos.x - 200.0, player->pos.x + 200.0);
			draw_frame.projection = m4_make_orthographic_projection(background->scrollPos.x, background->scrollPos.y, 0.0, 300.0, -1, 10);
			// animateF32ToTarget(&(background->scrollPos.x), player->pos.x +, deltaTime, rate);
		}

		setAnimation(player, a_walk, nowTime);
		
	}
	else if (player->isMoving)
	{
		player->isMoving = false;
		setAnimation(player, a_idle, nowTime);
		log("Stopped Moving. POS: %v2", player->pos);
	}


}

void movePlayerToObject(Entity* player, Entity* object, WorldFrame worldF)
{	
	player->destPos = object->interactPos;
	player->isMoving = true;
	movePlayer(player, worldF.bg, worldF.nowTime, worldF.deltaTime);
}


void movePlayerToClick(Entity* player, WorldFrame worldF)
{
	// get valid destination pos from mousPos
	player->destPos = worldF.mousePosWorld;
	player->isMoving = true;
	movePlayer(player, worldF.bg, worldF.nowTime, worldF.deltaTime);	
}

