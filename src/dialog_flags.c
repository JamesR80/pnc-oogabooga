#define D_HAS_MET 1
#define D_REPEAT 2


void checkDialogFlags(Entity* actor, s32 flag)
{
	switch (flag)
	{
		case D_HAS_MET: 
			world->actorR->dialogID = world->actorR->lastLineID + 1;
        	break;

        case D_REPEAT:
            world->actorR->dialogID = world->actorR->lastLineID;
			break;

		default:
			break;
	}
}