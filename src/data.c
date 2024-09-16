
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
	s_bg_cargo,
	s_bg_luggage,
	s_bg_sleeper,
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
	bool onCursor;
	string name;
	Gfx_Image* image; 
	Vector2 size;
	Vector2 origin;
	string lookText;
	string useText;
	bool justClicked;
	Vector2 pos;
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

typedef enum RoomID
{
	r_nil = 0,
	r_cargo,
	r_dining,
	r_hallway,
	r_lounge,
	r_luggage,
	r_sleeper,
	//..
	r_MAX,
} RoomID;

typedef struct Room
{
	// .. map of sprites w pos, background, walkbox, etc
} Room;

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

typedef enum WalkboxID
{
	w_nil = 0,
	w_dining_1,
	w_dining_2,
	w_dining_3,
	w_dining_4,
	//
	w_MAX,
} WalkboxID;

typedef struct BoxSides
{
	bool left;
	bool right;
	bool top;
	bool bottom;
} BoxSides;

typedef enum QuadType
{
	q_nil = 0,
	q_walkbox,
	q_door,
	q_object,
	//
	q_MAX,
} QuadType;

typedef struct Quad
{
	Vector2 q1; // bottom left
	Vector2 q2; // bottom right
	Vector2 q3; // top right
	Vector2 q4; // top left
	BoxSides isSideOpen;
	QuadType quadType;
} Quad;

typedef struct Walkbox
{
	Range2f box;
	BoxSides isSideOpen;
} Walkbox;

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

typedef enum UXStateID 
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
	Cursor cursors[c_MAX];
	Walkbox walkboxes[w_MAX];
	Sprite sprites[s_MAX];
	// Room rooms[r_MAX];
	UXStateID uxStateID;				
	float inventoryAlpha; 			// this is randy caveman shit. not sure about this approach
	float inventoryAlphaTarget;		// this is randy caveman shit. not sure about this approach
	SpriteID currentBG;
	Entity* activeEntity;
	Item* activeItem;
	CursorID currentCursor;
	bool isHWCursor;
	bool mouseActive;
	string playerText;
	float64 textBoxTime;
	Range2f dialogueBox;
	Range2f gameBox;
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
	Walkbox* activeWalkbox;

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
	if (spriteID >= 0 && spriteID < s_MAX) return &world->sprites[spriteID];
	else return &world->sprites[0];
}

Cursor* getCursor(CursorID cursorID)
{
	if (cursorID >= 0 && cursorID < c_MAX) return &world->cursors[cursorID];
	else return &world->cursors[0];
}

Item* getItem(ItemID itemID)
{
	if (itemID >= 0 && itemID < i_MAX) return &world->inventory[itemID];
	else return &world->inventory[0];
}

Walkbox* getWalkbox(WalkboxID walkboxID)
{
	if (walkboxID >= 0 && walkboxID < i_MAX) return &world->walkboxes[walkboxID];
	else return &world->walkboxes[0];
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
		assert(image, "failed to load image")
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

		world->sprites[spriteID] = sprite;			// room->sprites[spriteID]?

		// assert?
}

void loadCursor(CursorID cursorID, string path)
{
	Cursor cursor;
	if (cursorID == 0) path = STR("missingTexture.png");
	if (world->isHWCursor)
	{	
		cursor.hwCursor = os_make_custom_mouse_pointer_from_file(path, 7, 31, get_heap_allocator());
	}
	else
	{
		Gfx_Image* image = load_image_from_disk(path, get_heap_allocator());
		cursor.image = image;
		cursor.size = v2(16.0, 16.0);
		cursor.clickSpot = v2(5, 10);

	}

	world->cursors[cursorID] = cursor;	

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
	item.onCursor = false;
	item.lookText = STR("I'm looking at this item");
	item.useText = STR("I'm trying to use this item");
	item.pos = v2(0.0, 0.0);
	item.justClicked = false;


	world->inventory[itemID] = item;
} 

Walkbox boxMake(Vector2 min, Vector2 max, bool left, bool right, bool top, bool bottom)
{
	Walkbox walkbox;
	walkbox.box = range2f_make(min, max);
	walkbox.isSideOpen.left = left;
	walkbox.isSideOpen.right = right;
	walkbox.isSideOpen.top = top;
	walkbox.isSideOpen.bottom = bottom;
	return walkbox;
}

void loadWalkbox(WalkboxID walkboxID, Walkbox walkbox)
{	
	world->walkboxes[walkboxID] = walkbox;
}

void drawQuadLines(Quad quad, float lineWidth, Vector4 color)
{
	draw_line(quad.q1, quad.q2, lineWidth, color);
	draw_line(quad.q2, quad.q3, lineWidth, color);
	draw_line(quad.q3, quad.q4, lineWidth, color);
	draw_line(quad.q4, quad.q1, lineWidth, color);
}

float getAreaOfTriangle(Vector2 p1, Vector2 p2, Vector2 p3) 
{
    return (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0f;
}

bool isPointInConvexQuad(Quad quad, Vector2 p)
{	
    // Calculate the areas of the four triangles formed by the point and the vertices
    float area1 = getAreaOfTriangle(p, quad.q1, quad.q2);
    float area2 = getAreaOfTriangle(p, quad.q2, quad.q3);
    float area3 = getAreaOfTriangle(p, quad.q3, quad.q4);
    float area4 = getAreaOfTriangle(p, quad.q4, quad.q1);

    // Calculate the total area of the quadrilateral
    float totalArea = getAreaOfTriangle(quad.q1, quad.q2, quad.q3) + getAreaOfTriangle(quad.q1, quad.q3, quad.q4);

    // Check if the point is inside the quadrilateral
    bool inside = true;
    if (area1 < 0 || area2 < 0 || area3 < 0 || area4 < 0) {
        inside = false;
    } else if (fabsf(area1 + area2 + area3 + area4 - totalArea) > 0.001f) {
        inside = false;
    }

    return inside;
}
