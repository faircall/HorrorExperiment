

#ifndef _SDL_UTIL_H_
#include "SDL.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SDL.h"
#include "gmo_types.h"

#include <GL/glew.h>

typedef enum {
    SOFTWARE_RENDERER,
    OPENGL_RENDERER
} ActiveRenderer;

typedef enum {
    YUJI_TEX,
    OCEAN_TEX,
    LAKE_TEX,
    LAKE_FISHING_TEX,
    ROD_TEX,
    ROD_MID_TEX,
    RIPPLE_TEX,
    NUM_TEX,
} TextureType;

typedef enum {
    QUAD_SHADER,
    NUM_SHADERS,
} ShaderType;

typedef struct {
    byte *image;
    SDL_Texture *texture;
    int32 im_width;
    int32 im_height;
    int32 im_channels;
    bool loaded;
    TextureType type;
    uint32 gl_texture_id; 
} TextureResult;

typedef struct {
    SDL_Renderer *sdl_renderer;
    SDL_GLContext *gl_context;
    ActiveRenderer active_renderer;
    SDL_Window *window;
} GlobalRenderer;//can set to NULL if needed, which is nice

TextureResult load_texture(char *file_name, TextureType type, GlobalRenderer global_renderer);
TextureResult *load_textures(GlobalRenderer global_renderer);

#define _SDL_UTIL_H_
#endif
