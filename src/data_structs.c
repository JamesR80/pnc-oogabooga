// Redoing Data Structs
// Need World, Room, Objects, Characters, Text, Items
// Sprites (object, characters, items), Backgrounds, Cursors, Menus/UI
// State

#include "data_enums.c"

// :Sprites ( needed? or in Char/Obj?)
typedef struct Sprite
{
	Gfx_Image* image;
	Vector2 size;
	Vector2 clickableSize;
	Vector2 origin;
} Sprite;

typedef struct Background
{
	u64 stateFlags;
	u64 animFlags;
	
	SpriteID spriteID;
	Vector2 pos;
	Vector2 offset;
} Background;

typedef struct Foreground
{
	u64 stateFlags;
	u64 animFlags;
	
	SpriteID spriteID;
	Vector2 pos;
	Vector2 offset;
} Foreground;

Sprite sprites[s_MAX];

// :Text
typedef struct Text
{
	u64 languageFlag;
	Gfx_Font *font;
	u32 fontHeight;	
} Text;

// :Items - Inventory Items
typedef struct Item
{
	u64 flags;
	bool inInventory;
	string name;
	Gfx_Image* image; 
	// Vector2 size;
	// Vector2 origin;
	string lookString;
	string useString;
} Item;

Item items[i_MAX];


// :Characters
typedef struct Character
{
	u64 flags;
	u64 stateFlag; 	// PLAYER, NPC, VERB_LOOK, VERB_USE, etc
	u64 animFlag;	// PLAYING, IDLE, WALK_LEFT, PICK_UP,  etc

	string name;
	string hoverText;	// make all strings just an ID to a lookup DB. eg. EN_CHAR_LINE_XXX
	string lookText;
	string useText;
	// dialogue position

	Vector2 pos;
	Vector2 origin;
	SpriteID spriteID;
	// SpriteID portraitID;
	float speed;
	// int zindex;

} Character;

Character characters[c_MAX];


// :Objects - Objects are anything in the scene that is not a background/foreground or a character
typedef struct Object
{
	u64 stateFlag;	// CLICKABLE, LOOKABLE, USEABLE, etc
	u64 animFlag;	// PLAYING, ANIM1, ANIM2 etc

	bool isHotspot;

	RoomID roomID;

	string hoverText;
	string lookText;
	string useText;

	Vector2 pos;
	Vector2 origin;
	SpriteID spriteID;

	float interactRadius;
	Vector2 interactPos;
	// int zindex;

} Object;

Object objects[o_MAX];

typedef struct Walkbox
{
	Vector2 pos;
	Matrix4 box;
	bool inBox;
	bool active;
} Walkbox;




// :Rooms - a scene
typedef struct Room
{
	u64 stateFlag;

	RoomID roomID;
	string name;
	string lookText;
	
	Background backgrounds[];
	Foreground foregrounds[];

	Object doors[];
	Object objects[];
	Character npcs[];

	Walkbox walkboxes[];

	// 
} Room;

Room rooms[r_MAX];

typedef struct World
{

	Room rooms[r_MAX];
	Item inventory[i_MAX];
	UXState uxState;				// this is randy caveman shit. not sure about this approach

} World;

World* world = 0;