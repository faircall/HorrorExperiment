#include "g_gameutils.h"

Entity entity_init(int32 id, vec2 position, vec2 heading, vec2 last_heading, vec2 velocity, vec2 acceleration, real32 acceleration_value, real32 max_speed, bool attacking, real32 health, SDL_Rect rect, BRAIN_STATE brain_state)
{
    Entity result;
    result.id = id;
    result.position = position;
    result.heading = heading;
    result.last_heading = last_heading;
    result.velocity = velocity;
    result.acceleration = acceleration;
    result.acceleration_value = acceleration_value;
    result.max_speed = max_speed;
    result.attacking = attacking;
    result.health = health;
    result.rect = rect;
    result.exists= true;
    result.brain_state= brain_state;
    return result;
}

SDL_Rect rect_init(real32 x, real32 y, real32 w, real32 h)
{
    SDL_Rect result;
    result.x = x;
    result.y = y;
    result.w = w;
    result.h = h;
    return result;
}

RectToSave rect_to_save_init(real32 x, real32 y, real32 w, real32 h)
{
    RectToSave result;
    SDL_Rect current;
    current.x = x;
    current.y = y;
    current.w = w;
    current.h = h;
    result.original_rect = current;
    result.potential_rect= current;
    return result;
}

WaterRipple water_ripple_init(real32 x, real32 y, int32 max_frames)
{
    WaterRipple result;
    result.location = vec2_init(x,y);
    result.timer = 0.0f;
    result.animation_playing = false;
    result.animation_frame = 0;
    result.max_frames = max_frames;
    return result;
}

bool collides(SDL_Rect object_rect, SDL_Rect geometry_rect)
{
    bool result = false;
    real32 radius = object_rect.w/2.0f;
    SDL_Rect minkowski_sum;
    minkowski_sum.x = geometry_rect.x;
    minkowski_sum.y = geometry_rect.y;
    minkowski_sum.w = geometry_rect.w;
    minkowski_sum.h = geometry_rect.h;
    minkowski_sum.w += radius;
    minkowski_sum.h += radius;
    minkowski_sum.x -= radius/2.0f;
    minkowski_sum.y -= radius/2.0f;
    real32 object_center_x = object_rect.x + radius;
    real32 object_center_y = object_rect.y + radius;
    if (object_center_x >= minkowski_sum.x &&
        object_center_x <= (minkowski_sum.x + minkowski_sum.w) &&
        object_center_y >= (minkowski_sum.y) &&
        object_center_y <= (minkowski_sum.y + minkowski_sum.h)) {
        result = true;
    }					   				      
    return result;
}

void strip_newline(char *string)
{
    //string needs to have space alloced first
    uint32 count = 0;
    while (string[count] != '\0') {
	if (string[count] == '\n') {
	    string[count] = '\0';
	    return;
	}
	count++;
    }
}

void get_string_stripped(char *string, uint32 len)
{
    //string needs to have space alloced first
    string = fgets(string, len, stdin);
    strip_newline(string);
}
