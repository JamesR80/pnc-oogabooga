

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

void movePlayer(Entity* player, float64 nowTime, float64 deltaTime)
{
	Vector2 direction = v2_sub(player->destPos, player->pos);
	float distance = v2_length(direction);

	if (distance > 1.0f && player->isMoving)
	{	
		direction = v2_divf(direction, distance);
		float movement = player->speed * deltaTime;
		player->pos.x += direction.x * movement;
		player->pos.y += direction.y * movement;
		setAnimation(player, a_walk, nowTime);
		
	}
	else if (player->isMoving)
	{
		player->isMoving = false;
		setAnimation(player, a_idle, nowTime);
		log("Stopped Moving");
	}


}

void movePlayerToObject(Entity* player, Entity* object, float64 nowTime, float64 deltaTime)
{	
	player->destPos = object->interactPos;
	player->isMoving = true;
	movePlayer(player, nowTime, deltaTime);
}


void movePlayerToClick(Entity* player, Vector2 mousePos, float64 nowTime, float64 deltaTime)
{
	// get valid destination pos from mousPos
	player->destPos = mousePos;
	player->isMoving = true;
	movePlayer(player, nowTime, deltaTime);	
}