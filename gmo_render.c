#include "gmo_render.h"
#include "gmo_types.h"
#include "gmo_math.h"
#include "g_gamedefs.h"



/*
 *
 * More todo: Texture blitting using a UNIFORM for the texture-coords rather than
 * sending in the vertices up front. Slightly slower but much more flexible methinks?
 * basically sprite sheet rendering
 *
 * */

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
	SDL_Rect dest_rect = rect_init(dest_x - (float)width/2.0f, dest_y - (float)height/2.0f, width, height);
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[texture_type].texture, &source_rect, &dest_rect);
	
    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {
	//a little trickier cos of size

	//*almost* at parity, translation not quite right though
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	//glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	//glActiveTexture(GL_TEXTURE0);

	//remember 00 is screen centre...

	//some texture swimming happening here?
	//float x_translate = (2.0f*dest_x - SCREENWIDTH)/ ((float)SCREENWIDTH);
	float x_translate = (2.0f*dest_x - SCREENWIDTH) / ((float)SCREENWIDTH);
	//something something aspect ratio? I can just check the
	//SDL source code to see how *they* render. Nice.
	//
	//incorrect?
	//it's a little too low?
	float y_translate = (-2.0f*dest_y + SCREENHEIGHT) / ((float)SCREENHEIGHT);
	
	float x_scale_ratio = (float)game_resources.textures[texture_type].im_width / (float)game_resources.textures[texture_type].im_height;
	float x_scale = game_resources.textures[texture_type].im_width / (float)SCREENWIDTH;
	float y_scale = game_resources.textures[texture_type].im_height / (float)SCREENHEIGHT;
	//hrmm
	
	//Mat4 scale_transform = mat4_create_xyz_scale(x_scale, x_scale, 1.0f);

	//normal scaling seemed to be 11x the 64 * 64?
	
	Mat4 scale_transform = mat4_create_xyz_scale(y_scale, y_scale, 1.0f);
	Vec3 translation_vector = vec3_init(x_translate, y_translate, 1.0f);
	Mat4 translation = mat4_create_translation(translation_vector);
	Mat4 transform = mat4_mult(translation, scale_transform);
	//Mat4 transform = mat4_mult(scale_transform, translation);
	glUniformMatrix4fv(game_resources.shader_uniforms[QUAD_TRANSFORM_UNIFORM], 1, GL_FALSE, transform.elements);
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
	 *This will be hard to get right
	 *
	 * Not actually correct here!!!! */
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	//glActiveTexture(GL_TEXTURE0);
	float x_translate = (dest_x - SCREENWIDTH/2.0f)/ 2.0f*(float)SCREENWIDTH;
	float y_translate = (-dest_y + SCREENHEIGHT/2.0f) / 2.0f*(float)SCREENHEIGHT;
	float x_scale_ratio = (float)game_resources.textures[texture_type].im_width / (float)game_resources.textures[texture_type].im_height;
	//
	float x_scale = game_resources.textures[texture_type].im_width / (float)SCREENWIDTH;
	float y_scale = game_resources.textures[texture_type].im_height / (float)SCREENHEIGHT;
	Mat3 mat3_rotation = mat3_create_rotate_z(angle);
	Mat4 mat4_rotation = mat4_create_rotation(mat3_rotation);
	
	Mat4 scale_transform = mat4_create_xyz_scale(x_scale*x_scale_ratio, y_scale, 1.0f);
	Vec3 translation_vector = vec3_init(x_translate, y_translate, 1.0f);
	Mat4 translation = mat4_create_translation(translation_vector);
	Mat4 transform = mat4_mult(mat4_mult(mat4_rotation ,translation), scale_transform);
	glUniformMatrix4fv(game_resources.shader_uniforms[QUAD_TRANSFORM_UNIFORM], 1, GL_FALSE, transform.elements);
	
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
	Mat4 mat4_identity = mat4_create_identity();
	//what do we scale BY exactly?
	//just assume height is 'constant' and
	//scale by the change in width?
	float x_scale_ratio = ((float)game_resources.textures[texture_type].im_width)/((float)game_resources.textures[texture_type].im_height);//optimize by calculating this once upon creation and storing
	Mat4 scale_transform = mat4_create_xyz_scale(x_scale_ratio, 1.0f, 1.0f);
	Vec3 translation_vector = vec3_init(0.0f, 0.0f, x_scale_ratio);
	Mat4 translation = mat4_create_translation(translation_vector);
	Mat4 transform = mat4_mult(translation, scale_transform);
	glUniformMatrix4fv(game_resources.shader_uniforms[QUAD_TRANSFORM_UNIFORM], 1, GL_FALSE, transform.elements);
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

void draw_texture_fullscreen_with_normal(GlobalRenderer global_renderer, GameResource game_resources, TextureType texture_type, TextureType texture_type_normal, Vec3 light_pos)
{
    if (global_renderer.active_renderer == SOFTWARE_RENDERER) {
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[texture_type].texture, NULL, NULL);

    } else if (global_renderer.active_renderer == OPENGL_RENDERER) {

		/*
	 * Warning!!
	 * Placeholder!
	 * Not actually correct here!!!! */
	glUseProgram(game_resources.shaders[QUAD_SHADER]);
	Mat4 mat4_identity = mat4_create_identity();
	//what do we scale BY exactly?
	//just assume height is 'constant' and
	//scale by the change in width?
	float x_scale_ratio = ((float)game_resources.textures[texture_type].im_width)/((float)game_resources.textures[texture_type].im_height);//optimize by calculating this once upon creation and storing
	Mat4 scale_transform = mat4_create_xyz_scale(x_scale_ratio, 1.0f, 1.0f);
	Vec3 translation_vector = vec3_init(0.0f, 0.0f, x_scale_ratio);
	Mat4 translation = mat4_create_translation(translation_vector);
	Mat4 transform = mat4_mult(translation, scale_transform);
	glUniformMatrix4fv(game_resources.shader_uniforms[QUAD_TRANSFORM_UNIFORM], 1, GL_FALSE, transform.elements);
	glUniform3f(game_resources.shader_uniforms[QUAD_LIGHT_UNIFORM], light_pos.x, light_pos.y, light_pos.z);
	//glUniform1i(game_resources.shader_uniforms[QUAD_TEXTURE_UNIFORM], 0);
	//i I think it's gonna be this guy
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, game_resources.textures[texture_type].gl_texture_id);
	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(GL_TEXTURE_2D, game_resources.textures[texture_type_normal].gl_texture_id);
	//need a second texture here
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
