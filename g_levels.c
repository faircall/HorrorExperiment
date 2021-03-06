#include "g_levels.h"

void do_title(GameState *game_state, GlobalRenderer *global_renderer, GameResource game_resources, bool *g_running)
{
    	SDL_Event sdl_event;

	while (SDL_PollEvent(&sdl_event)) {
	    if (sdl_event.type == SDL_QUIT) {
		*g_running = false;
		break;
	    }
	}
	#if 0
	SDL_SetRenderDrawColor(sdl_renderer, 0xbf, 0x00, 0xaf, 0xff);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderCopy(sdl_renderer, game_resources.textures[OCEAN_TEX].texture, NULL, NULL);
	SDL_RenderPresent(sdl_renderer);
	#endif

	static real32 light_movement = 0.0f;
	static int32 light_direction = 1;
	light_movement += (game_state->dt * light_direction * 0.5f);
	
	if (light_movement > 1.0f) {
	    light_movement = 1.0f;
	    light_movement -= (game_state->dt * light_direction);
	    light_direction = -1;
	}
	if (light_movement < -1.0f) {
	    light_movement = -1.0f;
	    light_movement -= (game_state->dt * light_direction);
	    light_direction = 1;
	}

	Vec3 light = vec3_init(light_movement,  0.5f, 5.0f);
	
	//pass it into a uniform here
	//game_resources.shader_uniforms

	//how to load the normal map?
		
	set_background_color(*global_renderer, 0xbf, 0x00, 0xaf, 0xff);

	//todo animate the waves

	//hrmm
	

	//draw_texture_fullscreen(*global_renderer, game_resources, OCEAN_TEX);
	draw_texture_fullscreen_with_normal(*global_renderer, game_resources, OCEAN_TEX, OCEAN_TEX_NORMAL, light);

	
	update_screen(*global_renderer);
	//need a test here
	//wrap the below
	ALint al_source_state;
	alGetSourcei(game_resources.sound_sources[OCEAN_SOUND], AL_SOURCE_STATE, &al_source_state);
	//printf("al step source is definitely %d\n", al_step_source);
	if (al_source_state == AL_STOPPED) {
	    //printf("al step source is stopped %d\n", al_source);
	    alSourcePlay(game_resources.sound_sources[OCEAN_SOUND]);
	    al_source_state = AL_PLAYING;
	} else if (al_source_state == AL_PLAYING){
	    //printf("al step source is playing %d\n", al_source);
	} else if (al_source_state == AL_INITIAL){
	    //printf("al step source is initial %d\n", al_source);
	    alSourcePlay(game_resources.sound_sources[OCEAN_SOUND]);
	} else if (al_source_state == AL_PAUSED){
	    //printf("al step source is pausedd %d\n", al_source);
	}

	while (SDL_PollEvent(&sdl_event)) {
	    if (sdl_event.type == SDL_QUIT) {
		*g_running = false;
		break;
	    }
	    if (sdl_event.type == SDL_KEYDOWN) {
		if (sdl_event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		    game_state->current_state = GAME_LAKE;
		    alSourceStop(game_resources.sound_sources[OCEAN_SOUND]);
		}
	    }

    }
	    

	//render title

}

void do_lake(GameState *game_state, GlobalRenderer *global_renderer, GameResource game_resources, bool *g_running)
{
    	SDL_Event sdl_event;

	while (SDL_PollEvent(&sdl_event)) {
	    if (sdl_event.type == SDL_QUIT) {
		*g_running = false;
		break;
	    }
	    if (sdl_event.type == SDL_KEYDOWN) {
		if (sdl_event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		    //game_state->current_state = GAME_FISHING;
		    game_state->current_state = GAME_WALKING;
		    //alSourceStop(al_source);
		}
	    }
	}

	//todo: animate the water

	set_background_color(*global_renderer, 0xbf, 0x00, 0xaf, 0xff);
	draw_texture_fullscreen(*global_renderer, game_resources, LAKE_TEX);
	update_screen(*global_renderer);
	//need a test here
	//wrap the below
	ALint al_source_state;
	alGetSourcei(game_resources.sound_sources[LAKE_SOUND], AL_SOURCE_STATE, &al_source_state);
	//printf("al step source is definitely %d\n", al_step_source);
	if (al_source_state == AL_STOPPED) {
	    //printf("al step source is stopped %d\n", al_source);
	    alSourcePlay(game_resources.sound_sources[LAKE_SOUND]);
	    al_source_state = AL_PLAYING;
	} else if (al_source_state == AL_PLAYING){
	    //printf("al step source is playing %d\n", al_source);
	} else if (al_source_state == AL_INITIAL){
	    //printf("al step source is initial %d\n", al_source);
	    alSourcePlay(game_resources.sound_sources[LAKE_SOUND]);
	} else if (al_source_state == AL_PAUSED){
	    //printf("al step source is pausedd %d\n", al_source);
	}
	    

	//render title

}

void set_rod_rect(int32 mouse_x, int32 mouse_y, SDL_Rect *rect, SDL_RendererFlip *flip, int32 *draw_rod_mid)
{
    real32 width_epsilon = 7.0f;
    if (mouse_x < SCREENWIDTH/2) { //stretch to the left

	//make this a min
	rect->x = mouse_x;
	rect->w = SCREENWIDTH/2 - mouse_x;
	if (rect->w < width_epsilon) {
	    *draw_rod_mid = -1;
	}
	*flip = SDL_FLIP_HORIZONTAL;
	
    } else { //stretch to the right
	rect->x = SCREENWIDTH/2;
	//make this a min
	rect->w = mouse_x - SCREENWIDTH/2;
	if (rect->w < width_epsilon) {
	    *draw_rod_mid = 1;
	}
	*flip = SDL_FLIP_NONE;
    }
}

void do_fishing(GameState *game_state, GlobalRenderer *global_renderer, GameResource game_resources, bool *g_running)
{

    /*
     * REWRITE/CLEANUP THE RENDERING CODE HERE
     * 
     * */
    
    //there is a bug here about
    //occasionally getting stuck

    
    SDL_Event sdl_event;
    static int32 mouse_x;
    static int32 mouse_y;
    static real32 level_timer = 0.0f;

	
    static int32 fish_direction_x = 1;

    static bool ripple_animation_playing = false;
	
    while (SDL_PollEvent(&sdl_event)) {
	if (sdl_event.type == SDL_QUIT) {
	    *g_running = false;
	    break;
	}
	if (sdl_event.type == SDL_MOUSEMOTION) {
	    SDL_GetMouseState(&mouse_x, &mouse_y);
	}

	if (sdl_event.type == SDL_KEYDOWN) {
	    if (sdl_event.key.keysym.scancode == SDL_SCANCODE_R) {
		if (global_renderer->active_renderer ==SOFTWARE_RENDERER) {
		    global_renderer->active_renderer = OPENGL_RENDERER;
		} else {
		    global_renderer->active_renderer = SOFTWARE_RENDERER;
		}

		    //alSourceStop(al_source);
	    }
	    if (sdl_event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		game_state->current_state = GAME_WALKING;
		//alSourceStop(al_source);
	    }
	}
    }

    //fish logic
    real32 fish_speed = 100.0f;
    //should tie this to ripples
    real32 fish_max_x = 900.0f;
    real32 fish_min_x = 50.0f;
	

	
    game_state->fish_location.x += fish_speed * (game_state->dt) * fish_direction_x;
    if (game_state->fish_location.x >= fish_max_x || game_state->fish_location.x <= fish_min_x) {
	fish_direction_x *= -1;
	game_state->fish_location.x += fish_speed * (game_state->dt) * fish_direction_x;
    }


	
    for (int32 i = 0; i < 4; i++) {
	//replace with vec2 distance
	if (scalar_distance(game_state->fish_location.x, game_state->water_ripples[i].location.x) < 5.0f) {
	    //make this a particle system
	    if (game_state->water_ripples[i].animation_playing == false) {
		game_state->water_ripples[i].animation_playing = true;
		game_state->water_ripples[i].timer = 0.0f;
	    }
	}
	if (game_state->water_ripples[i].animation_playing) {
	    game_state->water_ripples[i].timer += game_state->dt;
	    if (game_state->water_ripples[i].timer > (1.0f/15.0f)) {
		game_state->water_ripples[i].animation_frame++;
		game_state->water_ripples[i].timer = 0.0f;
		if (game_state->water_ripples[i].animation_frame >= game_state->water_ripples[i].max_frames) {
		    game_state->water_ripples[i].animation_frame = 0;
		    game_state->water_ripples[i].animation_playing = false;
		}
	    }
	}
    }


    draw_texture_fullscreen(*global_renderer, game_resources, LAKE_FISHING_TEX);
	
    for (int32 i = 0; i < 4; i++) {
	if (game_state->water_ripples[i].animation_playing) {
	    SDL_Rect t_ripple_rect_src = rect_init(game_state->water_ripples[i].animation_frame * 32, 0, 32, 32);
	    SDL_Rect t_ripple_rect_dst = rect_init(game_state->water_ripples[i].location.x, game_state->water_ripples[i].location.y, 32*3, 32*3);
	    //we will flip this if the direction of the fish
	    //is negative
	    if (fish_direction_x == -1) {
		//SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[RIPPLE_TEX].texture, &t_ripple_rect_src, &t_ripple_rect_dst);
		draw_texture_at(*global_renderer, game_resources, RIPPLE_TEX, game_state->water_ripples[i].location.x, game_state->water_ripples[i].location.y, game_state->water_ripples[i].animation_frame * 32, game_state->water_ripples[i].animation_frame * 32, 1.0f, 1.0f);
	    } else {
		t_ripple_rect_dst = rect_init(game_state->water_ripples[i].location.x - 50.0f, game_state->water_ripples[i].location.y, 32*3, 32*3);
		//SDL_RenderCopyEx(global_renderer.sdl_renderer, game_resources.textures[RIPPLE_TEX].texture, &t_ripple_rect_src, &t_ripple_rect_dst, 180.0f, NULL, SDL_FLIP_VERTICAL);
		draw_texture_at(*global_renderer, game_resources, RIPPLE_TEX, game_state->water_ripples[i].location.x, game_state->water_ripples[i].location.y, game_state->water_ripples[i].animation_frame * 32, game_state->water_ripples[i].animation_frame * 32, 1.0f, 1.0f);
	    }
	    //SDL_RenderFillRect(sdl_renderer, &t_ripple_rect_dst);
	}
    }

	
    //blit rod and resize to give slightly 3d effect
    int32 draw_rod_mid = 0;
    SDL_Rect rod_move_rect = {0};

    rod_move_rect.y = SCREENHEIGHT/2;
    rod_move_rect.h = 166*3;//how to get this from the image itself?
    SDL_RendererFlip flip;
    set_rod_rect(mouse_x, mouse_y, &rod_move_rect, &flip, &draw_rod_mid);

    SDL_Rect fish_rect = rect_init(game_state->fish_location.x, game_state->fish_location.y, 20, 20);

    SDL_RenderFillRect(global_renderer->sdl_renderer, &fish_rect);
    draw_texture_at(*global_renderer, game_resources, YUJI_TEX, game_state->fish_location.x, game_state->fish_location.y, 0.0f, 0.0f, 1.0f, 1.0f);

    #if 0
    //redo with 3d model or somethign maybe?
    if (draw_rod_mid == 1) {
	SDL_Rect rod_mid_rect = rect_init(SCREENWIDTH/2 - 75, SCREENHEIGHT/2 + 6, 152, 166*3);
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[ROD_MID_TEX].texture, NULL, &rod_mid_rect);
    } else if (draw_rod_mid == -1) {
	SDL_Rect rod_mid_rect = rect_init(SCREENWIDTH/2 - 80 , SCREENHEIGHT/2 + 6, 152, 166*3);
	SDL_RenderCopy(global_renderer.sdl_renderer, game_resources.textures[ROD_MID_TEX].texture, NULL, &rod_mid_rect);
    } else {
	SDL_RenderCopyEx(global_renderer.sdl_renderer, game_resources.textures[ROD_TEX].texture, NULL, &rod_move_rect, 0, NULL, flip);
    }
    #endif
    
    update_screen(*global_renderer);
    //need a test here
    //wrap the below
    ALint al_source_state;
    alGetSourcei(game_resources.sound_sources[LAKE_SOUND], AL_SOURCE_STATE, &al_source_state);
    play_sound_source(game_resources.sound_sources[LAKE_SOUND]);
    //printf("al step source is definitely %d\n", al_step_source);
    if (al_source_state == AL_STOPPED) {
	//printf("al step source is stopped %d\n", al_source);
	alSourcePlay(game_resources.sound_sources[LAKE_SOUND]);
	al_source_state = AL_PLAYING;
    } else if (al_source_state == AL_PLAYING){
	//printf("al step source is playing %d\n", al_source);
    } else if (al_source_state == AL_INITIAL){
	//printf("al step source is initial %d\n", al_source);
	alSourcePlay(game_resources.sound_sources[LAKE_SOUND]);
    } else if (al_source_state == AL_PAUSED){
	//printf("al step source is pausedd %d\n", al_source);
    }
	    

    //render title

}

void do_walking(GameState *game_state, GlobalRenderer *global_renderer, GameResource game_resources, bool *g_running)
{
    // start off
    // with some static vars for prototyping?
    float dt = game_state->dt;
    static Vec2 player_position = {.x = 20.0f, .y = 200.0f};
    static Vec2 player_velocity = {.x = 0.0f, .y = 0.0f};
    float player_speed = 150.0f;
    float max_speed = 0.1f;
    Vec2 player_heading = {.x = 0.0f, .y = 0.0f};

    SDL_Event sdl_event;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) {
	player_heading.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_S]) {
	player_heading.y = 1.0f;
    }
    if (keys[SDL_SCANCODE_A]) {
	player_heading.x = -1.0f;
    }
    if (keys[SDL_SCANCODE_D]) {
	player_heading.x = 1.0f;
    }

    while (SDL_PollEvent(&sdl_event)) {
	if (sdl_event.type == SDL_QUIT) {
	    *g_running = false;
	    break;
	}
    }


    // something messed up with vertical directions?
    // possibly an OPENGL issue though
    // in particular, width/height are both -1 to 1, 
    // but height is much smaller
    //player_heading = vec2_normalize(player_heading);
    player_velocity = vec2_add(player_velocity, vec2_scale(player_heading, player_speed*dt));
    if (vec2_mag(player_velocity) >= max_speed) {
	player_velocity = vec2_normalize(player_velocity);
	player_velocity = vec2_scale(player_velocity, max_speed);
    }

    printf("current pos is, x: %f y: %f\n", player_position.x, player_position.y);


    player_position = vec2_add(player_position, player_velocity);
    Vec2 player_friction = vec2_scale(player_velocity, -1.5f);
    player_velocity = vec2_add(player_velocity, vec2_scale(player_friction,dt));
    //player_position = vec2_add(player_position, player_velocity);

    set_background_color(*global_renderer, 0xbf, 0x00, 0xaf, 0xff);
    //draw_texture_fullscreen(*global_renderer, game_resources, LAKE_FISHING_TEX);
    TextureResult grass_tex = game_resources.textures[GRASS_TEX];

    
    int tiles_across = SCREENWIDTH / grass_tex.im_width;
    int tiles_down = SCREENHEIGHT / grass_tex.im_height;

    //this totally tanks our framerate in OpenGL!!
    //need to learn the batched version I think
    #if 0
    for (int i = 0; i <= tiles_across; i++) {
	for (int j = 0; j <= tiles_down; j++) {
	    draw_texture_at(*global_renderer, game_resources, GRASS_TEX, i*grass_tex.im_width, j*grass_tex.im_height, 0.0f, 0.0f, 1.0f, 1.0f);
	}
    }
    #endif
    draw_fullscreen_tilemap(*global_renderer, game_resources, GRASS_TEX);
    
    draw_texture_at(*global_renderer, game_resources, YUJI_TEX, player_position.x, player_position.y, 0.0f, 0.0f, 1.0f, 1.0f);

    draw_shadow_texture_at(*global_renderer, game_resources, YUJI_TEX, player_position.x + 30.0f, player_position.y + 30.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    update_screen(*global_renderer);
    
    
}




