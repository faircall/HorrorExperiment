#include "gmo_render.h"
#include "gmo_types.h"
#include "gmo_math.h"
#include "g_gamedefs.h"



//start filling out some opengl stuff here
void draw_rect(real32 x, real32 y, real32 width, real32 height)
{

}


void set_background_color(GlobalRenderer global_renderer, byte red, byte green, byte blue, byte alpha)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	SDL_SetRenderDrawColor(global_renderer.sdl_renderer, red, green, blue, alpha);
	SDL_RenderClear(global_renderer.sdl_renderer);
    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {
	float r, g, b, a;
	r = ((float)red) / 255.0f;
	g = ((float)green) / 255.0f;
	b = ((float)blue) / 255.0f;
	a = ((float)alpha) / 255.0f;
	glClearColor(r, g, b, a);//of course this takes floats
	glClear(GL_COLOR_BUFFER_BIT);
    }
}

void draw_texture_at(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, float dest_x, float dest_y, float source_x, float source_y, float scale_x, float scale_y)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	int32 width = game_resources.textures[texture_type].im_width;
	int32 height = game_resources.textures[texture_type].im_height;
	SDL_Rect source_rect = rect_init(source_x, source_y, width, height);
	SDL_Rect dest_rect = rect_init(dest_x, dest_y, width, height);
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[texture_type].texture, &source_rect, &dest_rect);
	
    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	//glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_resources.textures[texture_type].gl_texture_id);
	glBindVertexArray(game_resources.vaos[QUAD_VAO]);
	//texture location?
	//elemetn count?
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void draw_rotated_texture_at(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, float dest_x, float dest_y, float source_x, float source_y, float scale_x, float scale_y, float angle)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	//width, height?
	int32 width = game_resources.textures[texture_type].im_width;
	int32 height = game_resources.textures[texture_type].im_height;
	SDL_Rect source_rect = rect_init(source_x, source_y, width, height);
	SDL_Rect dest_rect = rect_init(dest_x, dest_y, width, height);
	SDL_RenderCopyEx(global_renderer.sdl_renderer, game_resources.textures[texture_type].texture, &source_rect, &dest_rect, angle, NULL, SDL_FLIP_VERTICAL);
	
    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {
	/*
	 * Warning!!
	 * Placeholder!
	 * Not actually correct here!!!! */
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_resources.textures[texture_type].gl_texture_id);
	glBindVertexArray(game_resources.vaos[QUAD_VAO]);
	//texture location?
	//elemetn count?
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void draw_texture_fullscreen(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[texture_type].texture, NULL, NULL);

    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {

		/*
	 * Warning!!
	 * Placeholder!
	 * Not actually correct here!!!! */
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	//glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_resources.textures[texture_type].gl_texture_id);
	glBindVertexArray(game_resources.vaos[QUAD_VAO]);
	//texture location?
	//elemetn count?
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
    }
}

void update_screen(GlobalRenderer global_renderer)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	SDL_RenderPresent(global_renderer.sdl_renderer);
    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {
	SDL_GL_SwapWindow(global_renderer.window);
    }
}
