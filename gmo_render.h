#include "GL/glew.h" 

#include "sdl_util.h"

#include "gmo_types.h"

void set_background_color(GlobalRenderer global_renderer, byte red, byte green, byte blue, byte alpha);

void draw_texture_at(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, float x, float y, float scale_x, float scale_y);

void draw_texture_fullscreen(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type);

void update_screen(GlobalRenderer global_renderer);
