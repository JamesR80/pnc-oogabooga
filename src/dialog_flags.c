#define D_HAS_MET_CONDUCTOR 1
#define D_STAGNANT 2


void checkDialogFlags(Entity* actor, s32 flag)
{
	switch (flag)
	{
		case D_HAS_MET_CONDUCTOR: 
			world->actorR->dialogID = 202;
        	break;

        case D_STAGNANT:
            world->actorR->dialogID = world->actorR->lastLineID;
			break;

		default:
			break;
	}
}