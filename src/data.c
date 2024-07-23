typedef struct Sprite
{
	Gfx_Image* image;
	Vector2 size;
} Sprite;

typedef enum SpriteID
{
	s_nil = 0,
	s_player,
	s_room0,
	s_item0,
	s_obj0,
	s_door0,
	s_tree0,
	s_rock0,
	s_MAX,
} SpriteID;

Sprite sprites[s_MAX];

typedef enum EntityFlags
{
    ACTIVE = 1 << 0,
    //etc

} EntiityFlags;

typedef enum EntityType
{
	nil = 0,
	t_player = 1,
	t_npc = 2,
	t_room = 3,
	t_object = 4,
	t_item = 5,
	t_door = 6,
	t_tree = 7,
	t_rock = 8,
} EntityType;

typedef struct Entity // MegaStruct approach
{
	bool isValid;
	EntityType type;
    u64 flags;
	Vector2 pos;
	SpriteID spriteID;
	bool renderSprite;
} Entity;

Sprite* getSprite(SpriteID spriteID)
{
	if(spriteID >= 0 && spriteID < s_MAX) return &sprites[spriteID];
	return &sprites[0];
}

#define MAX_ENTITY_COUNT 1024

typedef struct World
{
	Entity entities[MAX_ENTITY_COUNT];
} World;

World* world = 0;

Entity* createEntity(EntityType type, SpriteID spriteID, Vector2 pos, Vector2 tileSize)
{
	Entity* entityFound = 0;
	for (int i = 0; i < MAX_ENTITY_COUNT; i++)
	{
		Entity* e = &world->entities[i];
		if (!e->isValid) // ie if no entity exists here, memory is free, use that
		{
			entityFound = e;
			break;
		}
	}
	assert(entityFound, "No more free entities");
	entityFound->isValid = true;
	entityFound->type = type;
	entityFound->pos = roundPosToTile(pos, tileSize);
	entityFound->pos.y -= (tileSize.y * 0.5);
	entityFound->spriteID = spriteID;
	return entityFound;
}

void destroyEntity(Entity* e)
{
	memset(e, 0, sizeof(Entity));
}
