

#ifndef _SDL_UTIL_H_
#include "SDL.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SDL.h"
#include "gmo_types.h"
#include "gmo_utils.h"
#include "gmo_sound.h"
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

typedef enum {
    QUAD_VAO,
    NUM_VAOS,
} VaoType;
//these should actually be coupeld together I think?
typedef enum {
    QUAD_VBO,
    NUM_VBOS,
} VboType;

typedef enum {
    QUAD_EBO,
    NUM_EBOS,
} EboType;

typedef enum {
    QUAD_PERSPECTIVE_UNIFORM,
    QUAD_TEXTURE_UNIFORM,
    NUM_SHADER_UNIFORMS
} ShaderUniformType;


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
    Mat4 perspective_matrix;
} GlobalRenderer;//can set to NULL if needed, which is nice

typedef struct {
    real32 *data;
    int32 length;
} VertexArray;

typedef struct {
    ALuint *sound_sources;
    ALuint *sound_buffers;
    TextureResult *textures;
    uint32 *shaders;
    int32 *shader_uniforms;
    uint32 *vaos;
    uint32 *vbos;
    uint32 *ebos;
} GameResource;


TextureResult load_texture(char *file_name, TextureType type, GlobalRenderer global_renderer);
TextureResult *load_textures(GlobalRenderer global_renderer);

uint32 load_shader(char *file_name, GLenum shader_type);

uint32 load_shader_program(char *file_name);

uint32 *create_vaos();

uint32 *load_vbos(GameResource game_resources);

uint32 *load_ebos(GameResource game_resources);

void set_vaos(GameResource game_resources);

int32 *load_shader_uniforms(GameResource game_resources);

#define _SDL_UTIL_H_
#endif
