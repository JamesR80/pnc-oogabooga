

void entityClicked(Entity* entity, Entity* player, bool isLeftClick) // entity clicked or just screen clicked?
{

	switch (isLeftClick)
	{
		case true: 
			entity->justClicked = true;
			log("left click!");
			if (fabsf(v2_dist(entity->pos, player->pos)) < entity->interactRadius) 
			{
				entity->interactable = true;
			}
			else
			{
				entity->interactable = false;
				// move to entity...
			} 
			break;

		case false:
			entity->justClicked = true;
			log("right click!");
			break;
	}
	if (entity->interactable)
	{	
		switch (entity->type)
		{
			case t_npc:
				world->playerText = entity->useText;
				world->uxStateID = ux_dialog;
				break;
			case t_object:
				world->playerText = entity->useText;
				world->inventory[entity->itemID].inInventory = true;
				destroyEntity(entity);
				break;
			case t_door:
				// do transition
				break;
			default:
				break;

		}
		
		// else e->useText = STR("I can't use it from here");
		// and set text flag...
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
	AnimType anim; // = getSprite(player->spriteID)->currentAnim;

	if (distance > 1.0f && player->isMoving)
	{	
		direction = v2_divf(direction, distance);
		float movement = player->speed * deltaTime;
		player->pos.x += direction.x * movement;
		player->pos.y += direction.y * movement;

		if (direction.x < 0) { anim = a_walk_left; }
		if (direction.x > 0) { anim = a_walk_right; }
		setAnimation(player, anim, nowTime);

		// clamp movement to walkbox
		if (player->pos.x < 90.0) player->pos.x = 90.0;
		if (player->pos.y < 100.0) player->pos.y = 100.0;
		if (player->pos.x > 610.0) player->pos.x = 610.0;
		if (player->pos.y > 150.0) player->pos.y = 150.0;

		// // scroll background
		// float32 bgScrollLimit = getSprite(background->spriteID)->image->width - 200;
		// if (background->isScrollable && player->pos.x >= 200.0 && player->pos.x <= bgScrollLimit)
		// {
		// 	background->scrollPos = v2(player->pos.x - 200.0, player->pos.x + 200.0);
		// 	// draw_frame.projection = m4_make_orthographic_projection(background->scrollPos.x, background->scrollPos.y, 0.0, 300.0, -1, 10);
		// 	// animateF32ToTarget(&(background->scrollPos.x), player->pos.x +, deltaTime, rate);
		// }

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
	// get best available interact Pos based on npc dir.
	player->destPos = object->interactPos; 
	player->isMoving = true;
	movePlayer(player, worldF.bg, worldF.nowTime, worldF.deltaTime);
}


void movePlayerToClick(Entity* player, WorldFrame worldF)
{
	// get valid destination pos from mousPos or from proj?
	player->destPos = worldF.mousePosWorld;
	// convert destPos to valid location in walkbox
	player->isMoving = true;
	movePlayer(player, worldF.bg, worldF.nowTime, worldF.deltaTime);	
}

