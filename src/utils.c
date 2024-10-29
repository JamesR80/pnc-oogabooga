#define m4_identity m4_make_scale(v3(1, 1, 1))

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

float sinBob(float time, float rate)
{
	return (sin(time * rate) + 1.0) / 2.0;
}

// int randFromRange(int min, int max)
// {
//   return (min + (rand() % (1 + max - min)));
// }

typedef struct Range1f {
  float min;
  float max;
} Range1f;
// ...

typedef struct Range2f {
  Vector2 min;
  Vector2 max;
} Range2f;

inline Range2f range2f_make(Vector2 min, Vector2 max) { return (Range2f) { min, max }; }

Range2f range2f_shift(Range2f r, Vector2 shift) {
  r.min = v2_add(r.min, shift);
  r.max = v2_add(r.max, shift);
  return r;
}

Range2f range2f_make_bottom_center(Vector2 size) {
  Range2f range = {0};
  range.max = size;
  range = range2f_shift(range, v2(size.x * -0.5, 0.0));
  return range;
}

Vector2 range2f_size(Range2f range) {
  Vector2 size = {0};
  size = v2_sub(range.min, range.max);
  size.x = fabsf(size.x);
  size.y = fabsf(size.y);
  return size;
}

bool range2f_contains(Range2f range, Vector2 v) {
  return v.x >= range.min.x && v.x <= range.max.x && v.y >= range.min.y && v.y <= range.max.y;
}

void animateV2ToTarget(Vector2* value, Vector2 target, float deltaTime, float rate)
{
	animateF32ToTarget(&(value->x), target.x, deltaTime, rate);
	animateF32ToTarget(&(value->y), target.y, deltaTime, rate);
}


Vector2 getMouseCurrentProj()
{
	//normalised device coords for the mouse
	float normX = (input_frame.mouse_x / (window.width * 0.5f)) - 1.0f;
	float normY = (input_frame.mouse_y / (window.height * 0.5f)) - 1.0f;
	Vector4 worldPos = v4(normX, normY, 0.0f, 1.0f);

	//transform to world coords
	worldPos = m4_transform(m4_inverse(draw_frame.projection), worldPos);
	worldPos = m4_transform(draw_frame.camera_xform, worldPos);

	return (Vector2){worldPos.x, worldPos.y};
}

inline float v2_dist(Vector2 a, Vector2 b)
{
    return v2_length(v2_sub(a, b));
}

Range2f getHotSpot(Vector2 size, Vector2 origin)
{
  Range2f range = {0};
  range.max = size;
  range = range2f_shift(range, v2_mulf(origin, -1.0));
  return range;
}

Vector2 getCenterRange2f(Range2f r)
{
	return (Vector2) { ((r.max.x - r.min.x) * 0.5 + r.min.x), ((r.max.y - r.min.y) * 0.5 + r.min.y)};
}

// not using - DONT USE THIS
Vector2 getUIPosFromWorldPos(Vector2 pos)
{
	Vector4 worldProj = v4(window.pixel_width * -0.5, window.pixel_width * 0.5, window.pixel_height * -0.5, window.pixel_height * 0.5);
	Vector4 UIProj = v4(0.0, 640.0, 0.0, 360.0);
	// can these be constants or the same? I hate this funciton
	
	float factorX = (2 * worldProj.y) / UIProj.y;
	float factorY = (2 * worldProj.w) / UIProj.w;
	pos.x = (pos.x + worldProj.y) / factorX;
	pos.y = (pos.y + worldProj.w) / factorY;
	return pos;
}

Vector2 centerTextToPos(string text, Gfx_Font* font, u32 fontHeight, Vector2 textScaling, Vector2 pos)
{
	Gfx_Text_Metrics metrics = measure_text(font, text, fontHeight, textScaling);
	Vector2 justified = v2_sub(pos, metrics.functional_pos_min);
	justified = v2_sub(justified, v2_divf(metrics.functional_size, 2.0));
	return justified;
}

void drawBoxFromRange2f(Range2f box, float lineWidth, Vector4 color)
{
	draw_line(box.min, v2(box.max.x, box.min.y) , lineWidth, color); // bottom of box
	draw_line(v2(box.max.x, box.min.y), box.max, lineWidth, color); // ridght side of box
	draw_line(box.max, v2(box.min.x, box.max.y), lineWidth, color); // top of box
	draw_line(v2(box.min.x, box.max.y), box.min, lineWidth, color); // left side of box

}

bool doLinesIntersect(Vector2 line1Start, Vector2 line1End, Vector2 line2Start, Vector2 line2End) // or return Vector2 if getting intersect point
{
  float epsilon = 0.1;
  float tolerance = 0.1;

  // Calculate the denominator of the intersection formula
  float denominator = (line1Start.x - line1End.x) * (line2Start.y - line2End.y) -
                      (line1Start.y - line1End.y) * (line2Start.x - line2End.x);

  // If the denominator is close to zero, the lines are parallel and do not intersect
  if (fabsf(denominator) < epsilon) return false; // // return (Vector2){NAN, NAN};

  // Calculate the intersection point
  float t = ((line1Start.x - line2Start.x) * (line2Start.y - line2End.y) -
              (line1Start.y - line2Start.y) * (line2Start.x - line2End.x)) / denominator;
  float u = -((line1Start.x - line1End.x) * (line1Start.y - line2Start.y) -
              (line1Start.y - line1End.y) * (line1Start.x - line2Start.x)) / denominator;

  // Check if the intersection point is within the line segments, with a tolerance
  bool line1Intersection = t >= -tolerance && t <= 1 + tolerance;
  bool line2Intersection = u >= -tolerance && u <= 1 + tolerance;

  // If the intersection point is within both line segments, return the intersection point
  if (line1Intersection && line2Intersection) 
  {
      float intersectionX = line1Start.x + t * (line1End.x - line1Start.x);
      float intersectionY = line1Start.y + t * (line1End.y - line1Start.y);
      // return (Vector2){intersectionX, intersectionY};
  }


  // Return a special "no intersection" value
  // return (Vector2){NAN, NAN};

  // do i want to return the line?

  // If the intersection point is within both line segments, return true
  return line1Intersection && line2Intersection;
}


