#include "sdl_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



char *g_texture_names[NUM_TEX] = {
    "art/yuji_rough.png",
    "art/ocean_bg.png",
    "art/the_lake.png",
    "art/fishing_temp.png",
    "art/fishing_pole_temp.png",
    "art/fishing_pole_middle_temp.png",
    "art/ripple-sheet.png"
};





TextureResult load_texture(char *file_name, TextureType type, GlobalRenderer global_renderer)
{
    //add opengl support into this
    //
    //load a 32-bit, rgba texture
    TextureResult texture_result;
    texture_result.type = type;
    texture_result.loaded = false;
    texture_result.image = stbi_load(file_name, &texture_result.im_width, &texture_result.im_height, &texture_result.im_channels, 0);
    if (texture_result.image == NULL) {
	//error handling
	return texture_result;
    }
    
    
    SDL_Surface *img_surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)texture_result.image, texture_result.im_width, texture_result.im_height, 32, 4*texture_result.im_width, SDL_PIXELFORMAT_RGBA32);
    if (img_surface == NULL) {
	//error handling
	return texture_result;
    }
    texture_result.texture = SDL_CreateTextureFromSurface(global_renderer.sdl_renderer, img_surface);
    SDL_FreeSurface(img_surface);
    if (texture_result.texture == NULL) {
	//error handling
	return texture_result;
    }

    glGenTextures(1, &(texture_result.gl_texture_id));
    glBindTexture(GL_TEXTURE_2D, texture_result.gl_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_result.im_width, texture_result.im_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_result.image);
    
    texture_result.loaded = true;
    return texture_result;
}

TextureResult *load_textures(GlobalRenderer global_renderer)
{
    TextureResult *textures = (TextureResult*)malloc(NUM_TEX * sizeof(TextureResult));
    for (int i = 0; i < NUM_TEX; i++) {
	textures[i] = load_texture(g_texture_names[i], i, global_renderer);
    }
    return textures;    
}
