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

char *g_shader_names[NUM_SHADERS] = {
    "shaders/quad"
};

char *g_shader_uniforms[NUM_SHADER_UNIFORMS] = {
    "perspective",
    "texture_to_draw",

    
};


#define QUAD_VERTEX_COUNT 20
real32 g_quad_vertices[QUAD_VERTEX_COUNT] = {
	//vertex 1, texture 1 bottom left
	-1.0f, -1.0f, 10.0f, 0.0f, 0.0f,
	//vertex 2, texture 2 bottom right
	1.0f, -1.0f, 10.0f, 1.0f, 0.0f,
	//vertex 3, texture 3 top left
	-1.0f, 1.0f, 10.0f, 0.0f, 1.0f,
	//vertex 4, texture 4 top right
	1.0f, 1.0f, 10.0f, 1.0f, 1.0f
};

#define QUAD_INDEX_COUNT 6
uint32 g_quad_indices[QUAD_INDEX_COUNT] = {
    0, 1, 2,
    2, 1, 3
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

uint32 load_shader(char *file_name, GLenum shader_type)
{
    char *source = read_file(file_name);
    uint32 result = glCreateShader(shader_type);
    glShaderSource(result, 1, &source, NULL);
    glCompileShader(result);
    int shader_success;
    char shader_info_log[512];
    glGetShaderiv(result, GL_COMPILE_STATUS, &shader_success);
    if (!shader_success) {
	glGetShaderInfoLog(result, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return 0; //this might be a problem no? i think must be non-zero for success tho
    }
    return result;
}

uint32 load_shader_program(char *file_name)
{
    char *vertex_file_name = string_concatenate(file_name, ".vert");
    char *fragment_file_name = string_concatenate(file_name, ".frag");
    uint32 vs = load_shader(vertex_file_name, GL_VERTEX_SHADER);
    uint32 fs = load_shader(fragment_file_name, GL_FRAGMENT_SHADER);
    
    uint32 result = glCreateProgram();
    glAttachShader(result, vs);
    glAttachShader(result, fs);
    glLinkProgram(result);

    int shader_success;
    char shader_info_log[512];
    glGetShaderiv(result, GL_LINK_STATUS, &shader_success);
    if (!shader_success) {
	glGetProgramInfoLog(result, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return 0; //this might be a problem no? i think must be non-zero for success tho
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return result;
    
}

uint32 *load_shader_programs(void)
{
    uint32 *result = (uint32*)malloc(sizeof(uint32) * NUM_SHADERS);
    for (int i = 0; i < NUM_SHADERS; i++) {
	result[i] = load_shader_program(g_shader_names[i]);
    }
    return result;
}

int32 *load_shader_uniforms(GameResource game_resources)
{
    //need to add a cross-reference table here (assoc. array?)
    int32 *result = (int32*)malloc(sizeof(int32) * NUM_SHADER_UNIFORMS);

    for (int i = 0; i < NUM_SHADER_UNIFORMS; i++) {
	//somethign like this
	if (i >= QUAD_PERSPECTIVE_UNIFORM && i <= QUAD_TEXTURE_UNIFORM) {
	    result[i] = glGetUniformLocation(game_resources.shaders[QUAD_SHADER], g_shader_uniforms[i]);
	} else {
	    return NULL;
	}

    }
    
    return result;
}
       

uint32 *create_vaos()
{
    uint32 *result = (uint32*)malloc(sizeof(uint32) * NUM_VAOS);
    for (int i = 0; i < NUM_VAOS; i++) {
	glGenVertexArrays(1, &result[i]);
	
    }
    return result;
}


//should probably roll the following into a single function
void set_vaos(GameResource game_resources)
{
    for (int i = 0; i < NUM_VAOS; i++) {
	glBindVertexArray(game_resources.vaos[i]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(real32), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(real32), (void*)3);
	glEnableVertexAttribArray(1);
    }
}

uint32 *load_ebos(GameResource game_resources)
{
    uint32 *result = (uint32*)malloc(sizeof(uint32) * NUM_EBOS);
    for (int i = 0; i < NUM_EBOS; i++) {
	glBindVertexArray(game_resources.vaos[i]);
	glGenBuffers(1, &result[i]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * QUAD_INDEX_COUNT, g_quad_indices, GL_STATIC_DRAW);
	
    }
    return result;
}

uint32 *load_vbos(GameResource game_resources)
{
    uint32 *result = (uint32*)malloc(sizeof(uint32) * NUM_VBOS);
    //temporarily just support one quad vbo, later switch to loading in the data
    for (int i = 0; i < NUM_VBOS; i++) {
	glBindVertexArray(game_resources.vaos[i]);
	glGenBuffers(1, &result[i]);
	glBindBuffer(GL_ARRAY_BUFFER, result[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(real32) * QUAD_VERTEX_COUNT, g_quad_vertices, GL_STATIC_DRAW);
	
	
    }

    return result;
}

void set_static_uniforms(GlobalRenderer global_renderer, GameResource game_resources)
{
    glUseProgram(game_resources.shaders[QUAD_SHADER]);
    glUniformMatrix4fv(game_resources.shader_uniforms[QUAD_SHADER], 1, GL_FALSE, global_renderer.perspective_matrix.elements);
}
