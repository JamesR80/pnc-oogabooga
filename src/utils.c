bool almostEquals(float a, float b, float epsillon) 
{
	return fabs(a-b) <- epsillon;
}

bool animateF32ToTarget(float* value, float target, float deltaTime, float rate)
{
	*value += (target - *value) * (1.0 - pow(2.0f, (-rate * deltaTime)));
	if (almostEquals(*value, target, 0.001f))
	{
		*value = target;
		return true;
	}
	return false;
}

void animateV2ToTarget(Vector2* value, Vector2 target, float deltaTime, float rate)
{
	animateF32ToTarget(&(value->x), target.x, deltaTime, rate);
	animateF32ToTarget(&(value->y), target.y, deltaTime, rate);
}

Vector2 getTilePos(Vector2 worldPos, Vector2 tileSize) 
{ 
	Vector2 tile = v2_div(worldPos, tileSize);
	return v2(roundf(tile.x), roundf(tile.y));
}
Vector2 getWorldPos(Vector2 tilePos, Vector2 tileSize) { return v2_mul(tilePos, tileSize); }
Vector2 roundPosToTile(Vector2 pos, Vector2 tileSize) { return getWorldPos(getTilePos(pos, tileSize), tileSize); } 
// Vector2i?


Vector2 getMouseInWorldCoords()
{
	//normalise the mouse coords
	float normX = (input_frame.mouse_x / (window.width * 0.5f)) - 1.0f;
	float normY = (input_frame.mouse_y / (window.height * 0.5f)) - 1.0f;
	Vector4 worldPos = v4(normX, normY, 0.0f, 1.0f);

	//transform to world coords
	worldPos = m4_transform(m4_inverse(draw_frame.projection), worldPos);
	worldPos = m4_transform(draw_frame.view, worldPos);

	return (Vector2){worldPos.x, worldPos.y};
}

// inline float v2_len(Vector2 a)
// {
//     return sqrt(a.x * a.x + a.y * a.y);
// }

// inline float v2_dist(Vector2 a, Vector2 b)
// {
//     return v2_len(v2_sub(a, b));
// }