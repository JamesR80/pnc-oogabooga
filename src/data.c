
typedef enum AnimType
{	
	a_nil = 0,
	a_idle,
	a_walk,
	//
	a_MAX,
} AnimType;

typedef struct Sprite
{
	Gfx_Image* image;
	Vector2 size;
	Vector2 clickableSize;
	Vector2 origin;
	u32 columns;
	u32 rows;
	u32 totalFrames;
    u32 frameWidth;
    u32 frameHeight;
	u32 animFPS;
	AnimType currentAnim;
	float64 animStartTime;
} Sprite;

typedef enum SpriteID
{
	s_nil = 0,
	s_player,
	s_ch_baron,
	s_ch_conductor,
	s_ch_detective,
	s_ch_professor,
	s_ch_reporter,
	s_ch_starlet,
	s_ch_valet,
	s_bg_lounge,
	s_bg_hallway,
	s_bg_dining,
	s_door0,
	s_MAX,
} SpriteID;
Sprite sprites[s_MAX];

typedef enum ItemID
{
	i_nil = 0,
	i_key,
	i_letter,
	i_wrench,
	i_rock,
	i_MAX,
} ItemID;

typedef struct Item
{
	u64 flags;
	bool inInventory;
	string name;
	Gfx_Image* image; 
	Vector2 size;
	Vector2 origin;
	string lookString;
	string useString;
} Item;

typedef enum LayerID
{
	l_nil = 0,
	l_background0,
	l_background1,
	l_background2,
	l_object0,
	l_character0,
	l_character1,
	l_character2,
	l_object1,
	l_foreground0,
	l_foreground1,
	l_zoom,
	l_map,
	l_inventory,
	l_journal,
	l_menu,
	l_MAX,
} LayerID;

typedef struct Room
{
	// .. map of sprites w pos, background, walkbox, etc
} Room;

typedef enum RoomID
{
	r_nil = 0,
	r_foyer,
	r_library,
	//..
	r_MAX,
} RoomID;
Room rooms[r_MAX];

typedef enum VerbState
{
	v_nil = 0,
	v_click,
	v_look,
	v_grab,
	v_use,  // --> use, open, pickup, talk, etc interact
	// etc...
	v_MAX,
} VerbState;

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
	t_background = 3,
	t_object = 4,
	t_item = 5,
	t_door = 6,
	t_MAX,
} EntityType;

typedef struct Entity // MegaStruct approach? Or Character, Room, Object, Background, Item, Animation Struct? etc
{
	bool isValid;
	EntityType type;
    u64 flags;
	Vector2 pos;
	Vector2 origin;
	Vector2 interactPos;
	Vector2 destPos;
	Vector2 scrollPos;
	SpriteID spriteID;
	ItemID itemID;
	RoomID roomID;
	bool clickable;
	string hoverText;
	string lookText;
	string useText;
	VerbState verbState;
	bool interactable;
	bool justClicked;
	bool isMoving;
	bool isAnimated;
	bool isScrollable;
	float64 speed;
	float64 interactRadius; // look radius?
} Entity;

#define MAX_ENTITY_COUNT 1024

typedef enum UXState 	// this is randy caveman shit. not sure about this approach
{
	ux_nil = 0,
	ux_inventory,
	ux_menu,
} UXState;

typedef struct World
{
	Entity entities[MAX_ENTITY_COUNT];
	Item inventory[i_MAX];
	// Room rooms[r_MAX];
	UXState uxState;				// this is randy caveman shit. not sure about this approach
	float inventoryAlpha; 			// this is randy caveman shit. not sure about this approach
	float inventoryAlphaTarget;		// this is randy caveman shit. not sure about this approach
} World;

World* world = 0;

typedef struct WorldFrame
{
	Entity* activeEntity;
	Item* activeItem; // or inventoryFrame?
} WorldFrame;

WorldFrame worldFrame;


Sprite* getSprite(SpriteID spriteID)
{
	if (spriteID >= 0 && spriteID < s_MAX) return &sprites[spriteID];
	else return &sprites[0];
}

Item* getItem(ItemID itemID)
{
	if (itemID >= 0 && itemID < i_MAX) return &world->inventory[itemID];
	else return &world->inventory[0];
}

Entity* createEntity(EntityType type, SpriteID spriteID, ItemID itemID, Vector2 pos, string hoverText, bool clickable, u64 flags) // flags - clickable, active, render etc..
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
	entityFound->flags = flags;
	entityFound->pos = pos;
	entityFound->spriteID = spriteID;
	entityFound->itemID = itemID;
	entityFound->clickable = clickable;
	entityFound->interactable = false;
	entityFound->justClicked = false;
	entityFound->interactRadius = 20.0f;
	entityFound->isMoving = false;
	entityFound->interactPos = pos;
	entityFound->destPos = pos;
	entityFound->isScrollable = true;
	entityFound->scrollPos = v2(0.0, 400.0);
	if (hoverText.count != 0) entityFound->hoverText = hoverText;
	else entityFound->hoverText = STR("");
	if (entityFound->type == t_player)  entityFound->verbState = v_look;
	else entityFound->verbState = v_nil;
	entityFound->lookText = STR("");
	entityFound->useText = STR("");

	return entityFound;
}

void destroyEntity(Entity* e)
{
	memset(e, 0, sizeof(Entity));
}

void loadRoom(RoomID roomID)
{
	// ..
}

void loadBackground() {} 	// TODO

void loadSprite(SpriteID spriteID, string path, Vector2 clickableSize, Vector2 origin, u32 cols, u32 rows) // no default values in c -> reason to use c++
{
		Sprite sprite;
		if (spriteID == 0) path = STR("missingTexture.png");
		Gfx_Image* image = load_image_from_disk(path, get_heap_allocator());
		// assert(image, "failed to load image")
		sprite.image = image;
		sprite.size = v2(sprite.image->width, sprite.image->height);

		if (clickableSize.x != 0.0)  sprite.clickableSize.x = clickableSize.x;
		else sprite.clickableSize.x = sprite.size.x;
		if (clickableSize.y != 0.0)  sprite.clickableSize.y = clickableSize.y;
		else sprite.clickableSize.y = sprite.size.y;

		if (origin.x) sprite.origin = origin;					// bottom center origin point of the hotspot relative to size
		else sprite.origin = v2(sprite.size.x * 0.5, 0.0);

		sprite.columns = cols;
		sprite.rows = rows;

		sprite.totalFrames = rows * cols;
    	sprite.frameWidth = image->width / cols;
    	sprite.frameHeight = image->height / rows;

		sprite.animFPS = 10;
		sprite.currentAnim = a_idle;
		sprite.animStartTime = 0;
		
		sprites[spriteID] = sprite;			// room->sprites[spriteID]?

		// assert?
}

void loadInventoryItem(ItemID itemID, string name, string path, u64 flags)
{
	Item item;
	// get the rest of this info from a database? so I can load it midgame?
	if (itemID == 0) path = STR("missingTexture.png");
	Gfx_Image* image = load_image_from_disk(path, get_heap_allocator());
	// assert(image, "failed to load image")
	item.image = image;
	item.size = v2(item.image->width, item.image->height);
	item.origin = v2(item.size.x * 0.5, 0.0);
	item.name = name;
	item.flags = flags;
	item.inInventory = false;

	world->inventory[itemID] = item;
} 

