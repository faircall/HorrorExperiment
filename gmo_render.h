#ifndef _GMO_RENDER_H_

#define _GMO_RENDER_H_

#include "GL/glew.h" 

#include "sdl_util.h"
#include "g_gameutils.h"
#include "gmo_types.h"
#include "g_gamedefs.h"
#include "SDL.h"

void set_background_color(GlobalRenderer global_renderer, byte red, byte green, byte blue, byte alpha);


void draw_texture_at(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, float dest_x, float dest_y, float source_x, float source_y, float scale_x, float scale_y);

void draw_texture_fullscreen(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type);

void draw_texture_fullscreen_with_normal(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, TextureType texture_type_normal, Vec3 light_pos);


void draw_rotated_texture_at(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, float dest_x, float dest_y, float source_x, float source_y, float scale_x, float scale_y, float angle);

void update_screen(GlobalRenderer global_renderer);


#endif
