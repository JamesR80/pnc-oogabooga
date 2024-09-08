// ID enums


typedef enum RoomID
{
	r_NIL = 0,
	r_LIBRARY,
	r_BEDROOM,

	//...
	r_MAX,
} RoomID;

typedef enum SpriteID
{
	s_nil = 0,
	s_player,
	s_obj0,
	s_door0,
	s_tree0,
	s_rock0,
	s_flower0,
	s_flower1,
	s_flower2,
	s_MAX,
} SpriteID;

typedef enum ItemID
{
	i_nil = 0,
	i_key,
	i_letter,
	i_wrench,
	i_rock,
	i_flower_pink,
	i_flower_blue,
	i_flower_gold,
	i_MAX,
} ItemID;

typedef enum CharacterID
{
	c_NIL = 0,
	c_PLAYER,
	c_LUCYCLAY,
	c_DETECTIVE,
	// ..
	c_MAX,
} CharacterID;

typedef enum ObjectID
{
	o_NIL = 0,

	//..
	o_MAX,
} ObjectID;

// PATHS to sprite data?