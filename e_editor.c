#include "e_editor.h"

/**
 * TODO:
 * Could add support for polygons rather than
 * just rects
 *
 *
 * TODO:
 * Start writing normal maps based on the image displayed
 * 
 * */

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, bool *g_running, EDITOR_STATE *editor_state);

void collision_update_and_render(SDL_Renderer *sdl_renderer, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv, EDITOR_STATE *editor_state, bool *want_save, bool *want_load);

void editor_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv)    
{

    static bool want_new_rect = false;
    static EDITOR_STATE editor_state = EDITOR_COLLISION;
    bool created_new_rect = false;
    bool want_save = false;    
    bool want_load = false;
    static RectToSave new_rect;// = rect_to_save_init(0.0f, 0.0f, 20.0f, 20.0f);

    //static texture_counter = 0;


    //this introduced WICKED lag
    //probably because I was polling event twice in one frame: a no-no
    if (editor_state == EDITOR_COLLISION) {
	collision_update_and_render(sdl_renderer, editor_rect_list, num_rects, rect_storage, mouse_x, mouse_y, g_running, argv, &editor_state, &want_save, &want_load);
    }

    if (editor_state == EDITOR_NORMAL_MAP) {
	normal_map_update_and_render(sdl_renderer, game_resources, g_running, &editor_state);
    }


    
    if (want_save) {
	//make this prettier and in-app rather than console
	char file_name_buffer[50];
	get_string_stripped(file_name_buffer, 50);
	FILE *fp = fopen(file_name_buffer, "wb");
	if (fp == NULL) {
	    //error handling
	    printf("couldn't open file\n");
	} else {		
	    fwrite(*editor_rect_list, 1, *rect_storage * sizeof(SDL_Rect), fp);
	}
	fclose(fp);
	/* possible to list directory contents?
	 * */
	TCHAR directory_buffer[MAX_PATH];
	DWORD dwRet;

	dwRet = GetCurrentDirectory(MAX_PATH, directory_buffer);
	if (dwRet == 0) {
	    printf("get current directory failed\n");
	}
	if (dwRet > MAX_PATH) {
	    printf("buffer too small for directory\n");
	}
	_tprintf(TEXT("directory was (%s) \n"), directory_buffer);
#if 0
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	hFind = FindFirstFileA(file_name_buffer, &ffd);

	if (hFind == INVALID_HANDLE_VALUE) {
	    printf("yeah hFind didn't work\n");
	}

#endif
	
    }

    if (want_load) {
	//make this prettier and in-app rather than console
	char file_name_buffer[50];
	get_string_stripped(file_name_buffer, 50);
	FILE *fp = fopen(file_name_buffer, "rb");
	if (fp == NULL) {
	    //error handling
	    printf("couldn't open file\n");
	} else {
	    //find size of file?
	    int32 size_of_file;
	    fseek(fp, 0, SEEK_END);
	    size_of_file = ftell(fp);
	    rewind(fp);
	    *editor_rect_list = (SDL_Rect*)realloc(*editor_rect_list, size_of_file);
	    fread(*editor_rect_list, size_of_file, 1, fp);
	}
	fclose(fp);
	
    }

    if (editor_state == EDITOR_NORMAL_MAP) {
	SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xbf, 0xaf, 0xff);
	SDL_RenderClear(sdl_renderer);
	
    }

    //SDL_RenderPresent(sdl_renderer);
}

void collision_update_and_render(SDL_Renderer *sdl_renderer, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv, EDITOR_STATE *editor_state, bool *want_save, bool *want_load)
{
     SDL_Event event;
     static bool want_new_rect = false;
     bool created_new_rect = false;
     static RectToSave new_rect;// = rect_to_save_init(0.0f, 0.0f, 20.0f, 20.0f);

     while (SDL_PollEvent(&event)) {
	 if (event.type == SDL_QUIT) {
	    *g_running = false;
	    break;
	 }
	 if (event.type == SDL_KEYUP) {
	     if (event.key.keysym.scancode == SDL_SCANCODE_S) {
		 *want_save = true;
	     }
	     if (event.key.keysym.scancode == SDL_SCANCODE_L) {
		 *want_load = true;
	     }

	     if (event.key.keysym.scancode == SDL_SCANCODE_N) {
		 *editor_state = EDITOR_NORMAL_MAP;
	     }

	     if (event.key.keysym.scancode == SDL_SCANCODE_C) {
		 *editor_state = EDITOR_COLLISION;
	     }
	 }
	 
	 if (event.type == SDL_MOUSEMOTION) {
	     SDL_GetMouseState(mouse_x, mouse_y);
	 }
	 if (event.type == SDL_MOUSEBUTTONDOWN) {

	    
	     if (event.button.button == SDL_BUTTON_LEFT) {
		 //make a vector from mouse pos
		 if (!want_new_rect) {
		     want_new_rect = true;
		     new_rect.original_rect = rect_init(*mouse_x, *mouse_y, 20.0f, 20.0f);
		 } else {
		     created_new_rect= true;		    
		 }
	     }
	     else if (event.button.button == SDL_BUTTON_RIGHT) {
		
	     }
	 }
	 
     }

     if (want_new_rect) {

	 if (created_new_rect) {
	     (*editor_rect_list)[*num_rects] = new_rect.potential_rect;
	     (*num_rects)++;
	     if (*num_rects >= *rect_storage) {
		 (*rect_storage) *= 2;
		 *editor_rect_list = (SDL_Rect*)realloc(*editor_rect_list, *rect_storage * sizeof(SDL_Rect));
	     }
	     want_new_rect = false;
	 } else {
	     //logic here is a little trippy
	     if (*mouse_x < new_rect.original_rect.x) {
		 if (*mouse_y < new_rect.original_rect.y) {
		     new_rect.potential_rect.x = *mouse_x;
		     new_rect.potential_rect.y = *mouse_y;
		     new_rect.potential_rect.w = new_rect.original_rect.x - *mouse_x;
		     new_rect.potential_rect.h = new_rect.original_rect.y - *mouse_y;
		 } else {
		     new_rect.potential_rect.x = *mouse_x;
		     new_rect.potential_rect.y = new_rect.original_rect.y;
		     new_rect.potential_rect.w = new_rect.original_rect.x - *mouse_x;
		     new_rect.potential_rect.h = *mouse_y - new_rect.original_rect.y;
		 }
	     } else {
		 if (*mouse_y < new_rect.original_rect.y) {
		     new_rect.potential_rect.x = new_rect.original_rect.x;
		     new_rect.potential_rect.y = *mouse_y;
		     new_rect.potential_rect.w = *mouse_x - new_rect.original_rect.x;
		     new_rect.potential_rect.h = new_rect.original_rect.y - *mouse_y;

		 } else {
		     new_rect.potential_rect.x = new_rect.original_rect.x;
		     new_rect.potential_rect.y = new_rect.original_rect.y;
		     new_rect.potential_rect.w = *mouse_x - new_rect.original_rect.x;
		     new_rect.potential_rect.h = *mouse_y - new_rect.original_rect.y;
		 }

	     }

	 }
     }

     SDL_SetRenderDrawColor(sdl_renderer, 0xbf, 0x00, 0xaf, 0xff);
     SDL_RenderClear(sdl_renderer);
    
     SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0xff, 0xff);
     if (want_new_rect) {
	 //blit the current status of new_rect
	 SDL_RenderFillRect(sdl_renderer, &new_rect.potential_rect);
     }
     SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0x00, 0xff);
     for (uint32 i = 0; i < (*num_rects); i++) {
	 SDL_RenderFillRect(sdl_renderer, &((*editor_rect_list)[i]));
     }
     SDL_RenderPresent(sdl_renderer);

}

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, bool *g_running, EDITOR_STATE *editor_state)
{
    SDL_Event event;
    static int texture_counter = 0;


    

    while (SDL_PollEvent(&event)) {
	if (event.type == SDL_QUIT) {
	    *g_running = false;
	    break;
	}

	if (event.type == SDL_KEYUP) {
	    if (event.key.keysym.scancode == SDL_SCANCODE_C) {
		*editor_state = EDITOR_COLLISION;
	    }
	    if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
		texture_counter++;
	    }
	    if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
		texture_counter--;
	    }
	}
    }

    if (texture_counter >= NUM_TEX) {
	texture_counter = 0;
    }
    if (texture_counter < 0) {
	texture_counter = NUM_TEX - 1;
    }

    //texture_counter = texture_counter % NUM_TEX;

    TextureResult texture_result = game_resources->textures[texture_counter];
    real32 scale = 3.0f;
    real32 dest_x = SCREENWIDTH/2.0f - scale*texture_result.im_width/2.0f;
    real32 dest_y = SCREENHEIGHT/2.0f - scale*texture_result.im_height/2.0f;
    SDL_Rect dest_rect = rect_init(dest_x, dest_y, scale*texture_result.im_width, scale*texture_result.im_height);

    SDL_SetRenderDrawColor(sdl_renderer, 0x0f, 0xff, 0xaf, 0xff);
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, texture_result.texture, NULL, &dest_rect);
    SDL_RenderPresent(sdl_renderer);
    
}

