

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
			// move player to click?

			break;
	}
	// some delay set justClicked to false.
}
