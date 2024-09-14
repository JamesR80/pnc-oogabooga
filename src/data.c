
typedef enum AnimType
{	
	a_nil = 0,
	a_idle,
	a_walk_left,
	a_walk_right,
	a_walk_down,
	a_walk_up,
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
	s_item_key,
	s_item_coupon,
	s_item_drink,
	s_item_headshot,
	s_po_player,
	s_po_baron,
	s_po_conductor,
	s_po_detective,
	s_po_professor,
	s_po_reporter,
	s_po_starlet,
	s_po_valet,
	s_MAX,
} SpriteID;
Sprite sprites[s_MAX];

typedef enum ItemID
{
	i_nil = 0,
	i_key,
	i_coupon,
	i_drink,
	i_headshot,
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
	v_talk,
	v_use,  // --> use, open, pickup, etc interact
	// etc...
	v_MAX,
} VerbState;

typedef enum CursorID
{
	c_nil = 0,
	c_windows,
	c_click,
	c_look,
	c_talk,
	c_grab,
	c_grab2,
	c_use,
	c_drag,
	c_left,
	c_right,
	c_walk,
	c_MAX,
} CursorID;

typedef struct Cursor
{
	Gfx_Image* image;
	Custom_Mouse_Pointer hwCursor;
	CursorID cursorID;
	Vector2 size;
	Vector2 clickSpot;

} Cursor;
Cursor cursors[c_MAX];

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
	bool isInRangeToInteract;
	bool justClicked;
	bool isMoving;
	bool isAnimated;
	bool isScrollable;
	float64 speed;
	float64 interactRadius; // look radius?
	CursorID hoverCursor;
} Entity;

#define MAX_ENTITY_COUNT 1024

typedef enum UXStateID 	// this is randy caveman shit. not sure about this approach
{
	ux_nil = 0,
	ux_inventory,
	ux_dialog,
	ux_menu,
} UXStateID;


typedef struct World
{
	Entity entities[MAX_ENTITY_COUNT];
	Item inventory[i_MAX];
	// Room rooms[r_MAX];
	UXStateID uxStateID;				// this is randy caveman shit. not sure about this approach
	float inventoryAlpha; 			// this is randy caveman shit. not sure about this approach
	float inventoryAlphaTarget;		// this is randy caveman shit. not sure about this approach
	SpriteID currentBG;
	Entity* activeEntity;
	CursorID currentCursor;
	bool isHWCursor;
	bool mouseActive;
	string playerText;
	float64 textBoxTime;
	Range2f dialogueBox;
	bool debugOn;
} World;

World* world = 0;

typedef struct WorldFrame
{
	Entity* activeEntity;
	Entity* bg;
	Item* activeItem; // or inventoryFrame?
	bool onEntity;
	bool onItem;
	float64 nowTime;
	float64 deltaTime;
	Matrix4 world_proj;
	Matrix4 world_view;
	Vector2 mousePosWorld;
	Vector2 mousePosScreen;
	Entity* player;

} WorldFrame;

WorldFrame worldFrame;

float screenWidth = 400.0;
float screenHeight = 300.0;

void set_screen_space() 
{
	draw_frame.camera_xform = m4_scalar(1.0);
	draw_frame.projection = m4_make_orthographic_projection(0.0, screenWidth, 0.0, screenHeight, -1, 10);
}
void set_world_space() 
{
	draw_frame.projection = worldFrame.world_proj;
	draw_frame.camera_xform = worldFrame.world_view;
}


Sprite* getSprite(SpriteID spriteID)
{
	if (spriteID >= 0 && spriteID < s_MAX) return &sprites[spriteID];
	else return &sprites[0];
}

Cursor* getCursor(CursorID cursorID)
{
	if (cursorID >= 0 && cursorID < c_MAX) return &cursors[cursorID];
	else return &cursors[0];
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
	entityFound->isInRangeToInteract = false;
	entityFound->justClicked = false;
	entityFound->interactRadius = 35.0f;
	entityFound->isMoving = false;
	entityFound->interactPos = pos;
	entityFound->destPos = pos;
	entityFound->isScrollable = true;
	entityFound->scrollPos = v2(0.0, 400.0);
	if (hoverText.count != 0) entityFound->hoverText = hoverText;
	else entityFound->hoverText = STR("");
	entityFound->lookText = STR("I'm looking at this");
	entityFound->useText = STR("I am interacting with this");

	switch (entityFound->type)
	{
		case t_background:
			entityFound->hoverCursor = c_click; 
			break;

		case t_door:
			entityFound->hoverCursor = c_left; 
			break;

		case t_npc:
			entityFound->hoverCursor = c_talk; 
			break;

		case t_object:
			entityFound->hoverCursor = c_grab;
			break;

		case t_item:
			entityFound->hoverCursor = c_grab; 

		case t_player:
			entityFound->verbState = v_look;  ///

		default:
			entityFound->hoverCursor = c_click;

	}
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

// if clickable size is 0 it will be set to image.size
void loadSprite(SpriteID spriteID, string path, Vector2 clickableSize, Vector2 origin, bool isAnimated, u32 cols, u32 rows)
{
		Sprite sprite;
		if (spriteID == 0) path = STR("missingTexture.png");
		Gfx_Image* image = load_image_from_disk(path, get_heap_allocator());
		// assert(image, "failed to load image")
		sprite.image = image;

		if (isAnimated)
		{
			sprite.columns = cols;
			sprite.rows = rows;
			sprite.totalFrames = rows * cols;
			sprite.frameWidth = image->width / cols;
			sprite.frameHeight = image->height / rows;
			sprite.size = v2(sprite.frameWidth, sprite.frameHeight);
			sprite.animFPS = 6;
			sprite.currentAnim = a_idle;
			sprite.animStartTime = 0;
		}
		else sprite.size = v2(image->width, image->height);

		if (clickableSize.x != 0.0)  sprite.clickableSize.x = clickableSize.x;
		else sprite.clickableSize.x = sprite.size.x;
		if (clickableSize.y != 0.0)  sprite.clickableSize.y = clickableSize.y;
		else sprite.clickableSize.y = sprite.size.y;

		if (origin.x) sprite.origin = origin;					// bottom center origin point of the hotspot relative to size
		else sprite.origin = v2(sprite.size.x * 0.5, 0.0);

		sprites[spriteID] = sprite;			// room->sprites[spriteID]?

		// assert?
}

void loadCursor(CursorID cursorID, string path)
{
	Cursor cursor;
	if (cursorID == 0) path = STR("missingTexture.png");
	if (world->isHWCursor)
	{	
		cursor.hwCursor = os_make_custom_mouse_pointer_from_file(path, 10, 20, get_heap_allocator());
	}
	else
	{
		Gfx_Image* image = load_image_from_disk(path, get_heap_allocator());
		cursor.image = image;
		cursor.size = v2(16.0, 16.0);
		cursor.clickSpot = v2(5, 10);

	}

	cursors[cursorID] = cursor;	

}
void loadInventoryItem(ItemID itemID, string name, string path, bool inInventory, u64 flags)
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
	item.inInventory = inInventory;

	world->inventory[itemID] = item;
} 

