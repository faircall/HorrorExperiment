#ifndef _G_GAMEUTILS_H_

#include "SDL.h"
#include "gmo_types.h"
#include "gmo_math.h"
#include "sdl_util.h"
#include "gmo_sound.h"
#include "g_gamedefs.h"

Entity entity_init(int32 id, Vec2 position, Vec2 heading, Vec2 last_heading, Vec2 velocity, Vec2 acceleration, real32 acceleration_value, real32 max_speed, bool attacking, real32 health, SDL_Rect rect, BRAIN_STATE brain_state);

SDL_Rect rect_init(real32 x, real32 y, real32 w, real32 h);
RectToSave rect_to_save_init(real32 x, real32 y, real32 w, real32 h);
bool collides(SDL_Rect object_rect, SDL_Rect geometry_rect);
WaterRipple water_ripple_init(real32 x, real32 y, int32 max_frames);
void strip_newline(char *string);
void get_string_stripped(char *string);

#define _G_GAMEUTILS_H_
#endif
