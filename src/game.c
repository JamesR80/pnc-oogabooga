void entityClicked(Entity* e, Entity* player) // entity clicked or just screen clicked?
{
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
			break;

		case v_use:
			log("using!");
			// if entity can be picked up && player is within range
			if (e->interactable)
			{
				world->inventory[e->itemID].inInventory = true;
				destroyEntity(e);
			}
			break;

		default:
			// move player to click?

			break;
	}
}
