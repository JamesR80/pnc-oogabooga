

void pushInventory(Item* item)
{
	for (int j = 0; j < i_MAX; j++)
	{
		if (world->currentInv[j] == 0)
		{
			world->currentInv[j] = item;
			break;
		}
	}
}

void popInventory(Item* item)
{
	for (int j = 0; j < i_MAX; j++)
	{
		if (world->currentInv[j] == item)
		{
			world->currentInv[j] = 0;
			break;
		}
	}
}

// do flatten array func??? Don't need it, I think... ???


void itemClicked(Item* item, bool isLeftClick)
{
	// add to cursor mousePosUI with an arrow and hide hwcursor
	// switch hwCursor to cursor version of item?
	// fuck maybe I just scale cursors up from 32 x 3?

	switch (isLeftClick)
	{
		case true: 
			item->justClicked = true;
			log("item left click!");
			if (world->itemOnCursor == null) 
			{
				item->inInventory = false;
				popInventory(item);
				item->onCursor = true;
				world->itemOnCursor = item;
			} // check if item already on cursor
			// else do item combine logic. Big old switch statement? 
			break;

		case false:
			item->justClicked = true;
			log("item right click!");
			if (world->itemOnCursor == item) // item->onCursor)
			{
				item->inInventory = true;
				pushInventory(item);
				item->onCursor = false;
				world->itemOnCursor = null;
			}
			else // normal right click on item
			{
				log("describe item");
				// init dialogue screen.
			}
			break;
	}
}

void itemUsedOnEntity(Item* item, Entity* entity)
{
	// if (entity->isInRangeToInteract)

	if (getItem(entity->useableItemID) == item)
	{
		// update cursor - do this on hover!!!

		// do action with item. How to find appropriate action?
		doItemAction(entity->useableItemID, entity, null);
		
	}
	else
	{
		// "that doesn't work here"
	}
}

void itemUsedOnObject(Item* item, Object* object)
{

}


void entityClicked(Entity* entity, Entity* player, bool isLeftClick) // entity clicked or just screen clicked?
{
	switch (isLeftClick)
	{
		case true: 
			entity->justClicked = true;
			log("left click!");

		case false:
			entity->justClicked = true;
			log("right click!");
			break;
	}
	if (entity->isInRangeToInteract)
	{	
		switch (entity->type)
		{
			case t_npc:
				log("dialog?");
				world->actorR = entity;
				world->dialogID = entity->dialogID;
				world->uxStateID = ux_dialog;
				break;
			case t_object:
				// world->playerText = entity->useText;
				world->textBoxTime = worldFrame.nowTime;
				world->inventory[entity->itemID].inInventory = true;
				pushInventory(&world->inventory[entity->itemID]);
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
void objectClicked(Object* object, Entity* player, bool isLeftClick) // entity clicked or just screen clicked?
{
	switch (isLeftClick)
	{
		case true: 
			object->justClicked = true;
			log("left click!");
			break;

		case false:
			object->justClicked = true;
			log("right click!");
			break;
	}
	if (object->isInRangeToInteract)
	{	
		switch (object->type)
		{
			case ot_door:
				if (object->warpBG != null)
				{
					world->warpPos = object->warpPos;
					world->warpBG = object->warpBG;
					world->screenFade.currentlyFadingOut = true;
				}
				else 
				{
					world->playerText = STR("Can't go this way");
					world->textTimer = worldFrame.nowTime;
				}

				break;
			case ot_npc:
				world->uxStateID = ux_dialog;
				break;
			case ot_object:
				// do transition
				break;

			default:
				break;
		}
	}
	else
	{
		switch (object->type)
		{
			case ot_newgame:
				world->warpBG = object->warpBG;
				world->warpPos = object->warpPos;
				world->screenFade.currentlyFadingOut = true;
				break;
			case ot_settings:
				world->warpBG = object->warpBG;
				world->uxStateID = ux_settings;
				break;
			case ot_quit:
				window.should_close = true;
				break;
			default:
				break;
		}
	}
		// else e->useText = STR("I can't use it from here");
		// and set text flag...
}

	// some delay set justClicked to false.

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
	// Maybe just don't have path finding?
	Vector2 direction = v2_sub(player->destPos, player->pos);
	float distance = v2_length(direction);
	AnimType anim; // = getSprite(player->spriteID)->currentAnim;

	// if (range2f_contains(worldFrame.activeWalkbox->box, player->destPos))
	// {
		if (distance > 1.0f && player->isMoving)
		{	
			direction = v2_divf(direction, distance);
			float movement = player->speed * deltaTime;
			player->pos.x += direction.x * movement;
			player->pos.y += direction.y * movement;

			// if (doLinesIntersect(player->pos, player->destPos, worldFrame.activeWalkbox->box.max, 
			// 					v2(worldFrame.activeWalkbox->box.max.x, worldFrame.activeWalkbox->box.min.y)))
			// 					{
			// 						// log("true");
			// 					}

			if (direction.x < 0) { anim = a_walk_left; }
			if (direction.x > 0) { anim = a_walk_right; }
			setAnimation(player, anim, nowTime);

			// clamp movement to current walkbox unless side open
			if (!worldFrame.activeWalkbox->isSideOpen.left && player->pos.x < worldFrame.activeWalkbox->box.min.x)
			{
				player->pos.x = worldFrame.activeWalkbox->box.min.x;
				player->destPos = player->pos;
			}
			if (!worldFrame.activeWalkbox->isSideOpen.bottom && player->pos.y < worldFrame.activeWalkbox->box.min.y)
			{
				player->pos.y = worldFrame.activeWalkbox->box.min.y;
				player->destPos = player->pos;
			}
			if (!worldFrame.activeWalkbox->isSideOpen.right && player->pos.x > worldFrame.activeWalkbox->box.max.x)
			{
				player->pos.x = worldFrame.activeWalkbox->box.max.x;
				player->destPos = player->pos;
			}
			if (!worldFrame.activeWalkbox->isSideOpen.top && player->pos.y > worldFrame.activeWalkbox->box.max.y)
			{
				player->pos.y = worldFrame.activeWalkbox->box.max.y;
				player->destPos = player->pos;
			}
		}
		else if (player->isMoving)
		{
			player->isMoving = false;
			setAnimation(player, a_idle, nowTime);
			log("Stopped Moving. POS: %v2", player->pos);
		}

}

void movePlayerToObject(Entity* player, Vector2 interactPos, WorldFrame worldF) // this is not an actual object
{	
	// get best available interact Pos based on npc dir.
	player->destPos = interactPos; 
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

