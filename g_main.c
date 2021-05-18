#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tchar.h>
#include "SDL.h"
#include "gmo_types.h"
#include "gmo_math.h"
#include "gmo_sound.h"

#include "sdl_util.h"
#include "g_gamedefs.h"
#include "g_levels.h"
#include "g_gameutils.h"
#include "e_editor.h"

//#include "gmo_render.h"
#include <windows.h>





/*
 * Todo: Some basic editor functionality would be
 * Creating an array of collision rectangles, and saving them to disk
 * The saving to disk part is somewhat difficult if we want to
 * be able to list directory contents. That *is* operating system specific I believe.
 *
 * I *think* I need to pass some info from argv into my editor function, to use
 * FindFirstFile for example
 * */


void editor_update_and_render(SDL_Renderer *sdl_renderer, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running);

void game_update_and_render(SDL_Renderer *sdl_renderer, Entity *player, bool *g_running, int32 *mouse_x, int32 *mouse_y);

void update_game_state(GameState *game_state, SDL_Renderer *sdl_renderer, GameResource game_resources, bool *g_running);



int main(int argc, char **argv)
{
    /* SDL Init */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
           SDL_Log("Didn't init: %s\n", SDL_GetError());
    }
    
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    
    if (SDL_CreateWindowAndRenderer(SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_RESIZABLE, &sdl_window, &sdl_renderer) != 0) {
        SDL_Log("Didn't create window: %s\n", SDL_GetError());
    }
    
    /* OpenAL init */
    ALCdevice *al_device;
    alGetError();
    al_device = alcOpenDevice(NULL);
    if (check_error()) {
        printf("error opening al device\n");
    }
    if (!al_device) {
        printf("didn't open al device\n");
    }    
    ALCcontext *al_context;
    al_context = alcCreateContext(al_device, NULL);
    if (check_error()) {
        printf("error opening al context\n");
    }
    if (!alcMakeContextCurrent(al_context)) {
        printf("couldn't make Al context current\n");
    }
    
    /* load some images */
    GameResource game_resources;
    game_resources.textures = load_textures(sdl_renderer);
    game_resources.sound_sources = load_static_sources(al_context);
    game_resources.sound_buffers = load_sound_buffers(al_context);
    /* load sounds*/

    //wrap this
    ALfloat listener_orientation[] = {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f};     
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);//this could be updated according to player pos
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);//this could be updated according to player velocity
    alListenerfv(AL_ORIENTATION, listener_orientation);

    //wrap this in a "load resource" function


    /* set some globals */
    bool g_running = true;
    real32 dt, total_time_ms = 0.0f;
    uint32 last_time = 0, current_time = 0;

    int32 mouse_x, mouse_y;
    mouse_x = mouse_y = 0;


    //pull this out to a game_state init function
    GameState game_state;
    GAME_STATE starting_state = GAME_EDITOR;
    game_state.current_state = starting_state;
    game_state.current_score = 0;
    game_state.high_score = 0;
    game_state.fish_location = vec2_init(150.0f, 600.0f);
    game_state.water_ripples = (WaterRipple*)malloc(4 * sizeof(WaterRipple));
    //ripple has 11 frames, 32x32
    game_state.water_ripples[0] = water_ripple_init(150.0f, 600.0f, 11);
    game_state.water_ripples[1] = water_ripple_init(300.0f, 600.0f, 11);
    game_state.water_ripples[2] = water_ripple_init(450.0f, 600.0f, 11);
    game_state.water_ripples[3] = water_ripple_init(600.0f, 600.0f, 11);
    /* eventually create arrays for entities and maybe geometry */

    uint32 num_rects = 0;
    uint32 rect_storage = 10; //max 10 rects initially, this will grow 
    SDL_Rect *editor_rect_list = (SDL_Rect*)malloc(sizeof(SDL_Rect) * rect_storage);

    
    Entity player = entity_init(1, vec2_init(40.0f, 40.0f), vec2_init(0.0f, 0.0f),
				vec2_init(0.0f, 0.0f), vec2_init(0.0f, 0.0f), vec2_init(0.0f, 0.0f),
				3000.0f, 700.0f, false, 100.0f, rect_init(40.0f, 40.0f, 32.0f, 32.0f), BRAIN_RESTING);
    
    while (g_running) {
	current_time = SDL_GetTicks();
	total_time_ms = current_time / 1000.0f;
	dt = (current_time - last_time)/1000.0f;
	last_time = current_time;
	game_state.dt = dt;
	game_state.total_time_ms = total_time_ms;
	if (game_state.current_state != GAME_EDITOR) {
	update_game_state(&game_state, sdl_renderer, game_resources, &g_running);
	} else {
	    editor_update_and_render(sdl_renderer, &game_resources, &editor_rect_list, &num_rects, &rect_storage, &mouse_x, &mouse_y, &g_running, argv[1]);
	}
	
	#if 0
	SDL_Event sdl_event;
	//would like an array of function pointers to switch states rather than endless if statements

	//update_game_state(&game_state, game_resources, &g_running);
	
	if (game_state.current_state == GAME_TITLE) {
	    do_title(&game_state, al_source, sdl_renderer, textures, &g_running);	    
	} else if (game_state.current_state == GAME_OVER) {
	    while (SDL_PollEvent(&sdl_event)) {
		if (sdl_event.type == SDL_QUIT) {
		    g_running = false;
		    break;
		}
	    }
	    //render gameover
	} else if (game_state.current_state == GAME_LAKE) {
	    do_lake(&game_state, lake_source, sdl_renderer, textures, &g_running);
	} else if (game_state.current_state == GAME_FISHING) {
	    do_fishing(&game_state, lake_source, sdl_renderer, textures,  &g_running);
	} else if (game_state.current_state == GAME_PLAYING) {	    
	    game_update_and_render(sdl_renderer, &player, &g_running, &mouse_x, &mouse_y);	    
	} else if (game_state.current_state == GAME_EDITOR) {
	    editor_update_and_render(sdl_renderer, &editor_rect_list, &num_rects, &rect_storage, &mouse_x, &mouse_y, &g_running, argv[1]);
	}
	#endif
	
    }

    SDL_Quit();
    return 0;
}



void game_update_and_render(SDL_Renderer *sdl_renderer, Entity *player, bool *g_running, int32 *mouse_x, int32 *mouse_y)
{
    SDL_Event sdl_event;
    /* timer stuff */
    uint32 current_time = SDL_GetTicks();
    static last_time = 0;
    real32 dt = ((float)(current_time - last_time))/1000.0f;
    last_time = current_time;
        
    /* reset some flags*/
    player->attacking = false;
    player->heading = vec2_init(0.0f, 0.0f);
    real32 friction_value = 1800.f;
    real32 velocity_epsilon = 1.0f;
    /* event handling */

    while (SDL_PollEvent(&sdl_event)) {
	if (sdl_event.type == SDL_QUIT) {
	    *g_running = false;
	    break;
	}
	if (sdl_event.type == SDL_KEYDOWN) {
	    if (sdl_event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		player->attacking = true;
	    }
	}
	if (sdl_event.type == SDL_MOUSEMOTION) {
	    SDL_GetMouseState(mouse_x, mouse_y);
	}
	if (sdl_event.type == SDL_MOUSEBUTTONDOWN) {
	    //make a vector from mouse pos
                
	}
    }
        
    /* get key state*/
    const byte *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_W]) {
	player->heading.y = -1.0f;
	player->acceleration.y = -player->acceleration_value;
    }
    if (key_state[SDL_SCANCODE_S]) {
	player->heading.y = 1.0f;
	player->acceleration.y = player->acceleration_value;
    }
    if (key_state[SDL_SCANCODE_A]) {
	player->heading.x = -1.0f;
	player->acceleration.x = -player->acceleration_value;
            
    }
    if (key_state[SDL_SCANCODE_D]) {
	player->heading.x = 1.0f;
	player->acceleration.x = player->acceleration_value;	    
    }
        
    if (vec2_mag(player->heading) > 0.0f) {
	player->heading = vec2_normalize(player->heading);
	player->last_heading = player->heading;
    }
    player->acceleration = vec2_scale(player->acceleration_value, player->heading);
    player->velocity.x += player->acceleration.x * dt;
    player->velocity.y += player->acceleration.y * dt;
        
    real32 player_speed = vec2_mag(player->velocity);
    if (player_speed > velocity_epsilon) {
	vec2 friction = vec2_normalize(player->velocity);
	friction = vec2_scale(-friction_value, friction);
	player->velocity.x += friction.x * dt;
	player->velocity.y += friction.y * dt;
    }
        
    player_speed = vec2_mag(player->velocity);
    if (player_speed > player->max_speed) {
	player->velocity = vec2_normalize(player->velocity);
	player->velocity = vec2_scale(player->max_speed, player->velocity);
    }
        
    /* final integration */
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
        
        
    /* update appropriate rects */
    player->rect.x = player->position.x;
    player->rect.y = player->position.y;

        
    SDL_SetRenderDrawColor(sdl_renderer, 0xbf, 0x00, 0xaf, 0xff);
    SDL_RenderClear(sdl_renderer);
    SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0x00, 0xff);    
    
    SDL_RenderFillRect(sdl_renderer, &player->rect);
		
        
        
    /* update enemy brain here */

    SDL_RenderPresent(sdl_renderer);
}


void update_game_state(GameState *game_state, SDL_Renderer *sdl_renderer, GameResource game_resources, bool *g_running)
{
    switch (game_state->current_state) {
    case GAME_TITLE:
    {
	do_title(game_state, sdl_renderer, game_resources, g_running);
	break;
    }
    case GAME_OVER:
    {
	break;
    }
    case GAME_LAKE:
    {
	do_lake(game_state, sdl_renderer, game_resources, g_running);
	break;
    }
    case GAME_FISHING:
    {
	do_fishing(game_state, sdl_renderer, game_resources, g_running);
	break;
    }

    #if 0
    case GAME_PLAYING:
    {
	game_update_and_render(sdl_renderer, &player, &g_running, &mouse_x, &mouse_y);	    
	break;
    }
    case GAME_EDITOR:
    {
	editor_update_and_render(sdl_renderer, &editor_rect_list, &num_rects, &rect_storage, &mouse_x, &mouse_y, &g_running, argv[1]);
	break;
    }
    #endif
    }


}

