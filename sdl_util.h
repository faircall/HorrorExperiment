

#ifndef _SDL_UTIL_H_
#include "SDL.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SDL.h"
#include "gmo_types.h"

typedef enum {
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

TextureResult load_texture(char *file_name, TextureType type, SDL_Renderer *renderer);
TextureResult *load_textures(SDL_Renderer *renderer);

#define _SDL_UTIL_H_
#endif
