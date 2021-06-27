

#ifndef _SDL_UTIL_H_
#include "SDL.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SDL.h"
#include "gmo_types.h"

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

typedef struct {
    byte *image;
    SDL_Texture *texture;
    int32 im_width;
    int32 im_height;
    int32 im_channels;
    bool loaded;
    TextureType type;
} TextureResult;

typedef struct {
    SDL_Renderer *sdl_renderer;
    SDL_GLContext *gl_context;
} GlobalRenderer;//can set to NULL if needed, which is nice

TextureResult load_texture(char *file_name, TextureType type, GlobalRenderer global_renderer);
TextureResult *load_textures(GlobalRenderer global_renderer);

#define _SDL_UTIL_H_
#endif
