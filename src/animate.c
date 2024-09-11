
void setAnimation(Entity* entity, AnimType animType, float64 animStartTime)
{
    Sprite* spritesheet = getSprite(entity->spriteID);
    spritesheet->animStartTime = animStartTime;
    spritesheet->currentAnim = animType;

}

void animate(Entity* entity, float64 nowTime, float64 deltaTime)
{

    // if (entity.isAnimated) ?

    Sprite* spritesheet = getSprite(entity->spriteID); 
    Gfx_Image* image = spritesheet->image;

    u32 totalFrames = spritesheet->totalFrames;
    u32 frameWidth = spritesheet->frameWidth;
    u32 frameHeight = spritesheet->frameHeight;
    u32 startFrameX;
    u32 startFrameY;
    u32 endFrameX;
    u32 endFrameY;

    switch (spritesheet->currentAnim)
    {

        case a_nil:
            startFrameX = 0;
            startFrameY = 0;
            endFrameX = 0;
            endFrameY = 0;
            // assert()
            break;

        case a_idle:
            startFrameX = 0;
            startFrameY = 0;
            endFrameX = 3;
            endFrameY = 0;            
            break;

        case a_walk:
            startFrameX = 0;
            startFrameY = 0;
            endFrameX = 1;
            endFrameY = 0;           
            break;


        // Maybe save these on the Character data - or Entity
        default:
            break;
    }

    //assert() frames are valid?

    // frame index is the frame number independent of rows and columns.
	u32 animStartIndex = startFrameY * spritesheet->columns + startFrameX;  // 0 * 4 + 0 = 0  |  0 * 1 + 0
	u32 animEndIndex   = endFrameY   * spritesheet->columns + endFrameX;    // 0 * 4 + 3 = 3  |  0 * 1 + 0
	u32 animFrameTotal = max(animEndIndex, animStartIndex)-min(animEndIndex, animStartIndex) + 1;


    // Sanity check configuration
	assert(animEndIndex > animStartIndex, "The last frame must come before the first frame"); // 
	assert(startFrameX < spritesheet->columns, "anim_start_frame_x is out of bounds");
	assert(startFrameY < spritesheet->rows, "anim_start_frame_y is out of bounds");
	assert(endFrameX < spritesheet->columns, "anim_end_frame_x is out of bounds");
	assert(endFrameY < spritesheet->rows, "anim_end_frame_y is out of bounds");


    float32 timePerFrame = 1.0 / spritesheet->animFPS;
    float32 animDuration = timePerFrame * (float32)animFrameTotal;

    // Float modulus to "loop" around the timer over the anim duration
    float32 animElapsed = fmodf(nowTime - spritesheet->animStartTime, animDuration);
    
    // Get current progression in animation from 0.0 to 1.0
    float32 animProgFactor = animElapsed / animDuration;

    u32 animCurrentIndex = animFrameTotal * animProgFactor;
    u32 animAbsIndexInSheet = animStartIndex + animCurrentIndex;
    
    u32 animIndexX = animAbsIndexInSheet % spritesheet->columns;
    u32 animIndexY = animAbsIndexInSheet / spritesheet->columns + 1;
    
    u32 spritesheetPosX = animIndexX * frameWidth;
    u32 spritesheetPosY = (spritesheet->rows - animIndexY) * frameHeight; // Remember, Y inverted.

    // get pos to bottom middle of frame
    Matrix4 xform = m4_scalar(1.0);
    xform = m4_translate(xform, v3(entity->pos.x, entity->pos.y, 0));
	xform = m4_translate(xform, v3(frameWidth * -0.5, 0.0, 0));

    // Draw the sprite sheet, with the uv box for the current frame.
    // Uv box is a Vector4 of x1, y1, x2, y2 where each value is a percentage value 0.0 to 1.0
    // from left to right / bottom to top in the texture.
    Draw_Quad *quad = draw_image_xform(image, xform, v2(frameWidth, frameHeight), COLOR_WHITE);

    quad->uv.x1 = (float32)(spritesheetPosX) / (float32)image->width;
    quad->uv.y1 = (float32)(spritesheetPosY) / (float32)image->height;
    quad->uv.x2 = (float32)(spritesheetPosX + frameWidth) / (float32)image->width;
    quad->uv.y2 = (float32)(spritesheetPosY + frameHeight) / (float32)image->height;
    

}