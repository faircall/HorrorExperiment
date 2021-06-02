#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLMENTATION
#include "stb_image_write.h"

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

typedef struct {
    int grid_x;
    int grid_y;
} Mouse_Grid_Result;

Mouse_Grid_Result get_mouse_grid(int mouse_x, int mouse_y, uint grid_width, uint grid_height);

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, int32 *mouse_x, int32 *mouse_y, bool *g_running, EDITOR_STATE *editor_state);

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
	normal_map_update_and_render(sdl_renderer, game_resources, mouse_x, mouse_y, g_running, &editor_state);
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

Mouse_Grid_Result get_mouse_grid(int32 mouse_x, int32 mouse_y, int32 grid_spacing_x, int32 grid_spacing_y, int32 grid_start_x, int32 grid_start_y, int32 grid_end_x, int32 grid_end_y)
{
    /// a bit more think required
    Mouse_Grid_Result result;
    if (mouse_x < grid_start_x || mouse_y < grid_start_y || mouse_x > grid_end_x || mouse_y > grid_end_y) {
	result.grid_x = -1;
	result.grid_y = -1;
    } else {
    
	//we actually want to grid spacing...also
	result.grid_x = (mouse_x - grid_start_x) / grid_spacing_x;
	result.grid_y = (mouse_y - grid_start_y) / grid_spacing_y;
    }

    
    return result;
}

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, int32 *mouse_x, int32 *mouse_y, bool *g_running, EDITOR_STATE *editor_state)
{
    SDL_Event event;
    static int texture_counter = 0;

    static bool selecting = false;

    static vec2 selecting_start = {0.0f, 0.0f};
    static bool initialsed_normal_buffer = false;
    static bool *normal_buffer = NULL;
    


    //we could make a static pointer here
    //try it out for size 
    //static bool *normal_map_buffer = (bool*)malloc(sizeof());;

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

	if (event.type == SDL_MOUSEMOTION) {
	    SDL_GetMouseState(mouse_x, mouse_y);
	}
	if (event.type == SDL_MOUSEBUTTONUP) {

	    
	    if (event.button.button == SDL_BUTTON_LEFT) {
		//make a vector from mouse pos
		if (!selecting) {
		    selecting = true;
		    selecting_start = vec2_init(*mouse_x, *mouse_y);		    
		} else {
		    selecting = false;
		}
	    }
	    else if (event.button.button == SDL_BUTTON_RIGHT) {
		
	    }
	}
    }

    if (selecting) {
	
    }

    //need some kind of a select region function

    //how to select a particular cell??

    
    

    if (texture_counter >= NUM_TEX) {
	texture_counter = 0;
    }
    if (texture_counter < 0) {
	texture_counter = NUM_TEX - 1;
    }

    //texture_counter = texture_counter % NUM_TEX;

    TextureResult texture_result = game_resources->textures[texture_counter];
    //get grid-line info to render grid lines
    int32 scale = 8;
    //why minus
    int32 dest_x = (SCREENWIDTH/2) - (scale*texture_result.im_width/2);
    int32 dest_y = (SCREENHEIGHT/2) - (scale*texture_result.im_height/2);
    
    uint32 grid_width = texture_result.im_width * scale;
    uint32 grid_height = texture_result.im_height * scale;

    uint32 grid_start_x = dest_x;
    uint32 grid_end_x = grid_start_x + grid_width;

    uint32 grid_start_y = dest_y;
    uint32 grid_end_y = dest_y + grid_height;

    uint32 grid_lines_x = texture_result.im_width;
    uint32 grid_lines_y = texture_result.im_height;

    int mouse_grid_x, mouse_grid_y;

    Mouse_Grid_Result grid_result = get_mouse_grid(*mouse_x, *mouse_y, scale, scale, grid_start_x, grid_start_y, grid_end_x, grid_end_y);
    mouse_grid_x = grid_result.grid_x;
    mouse_grid_y = grid_result.grid_y;
    printf("grid x %d, grid y %d \n", mouse_grid_x, mouse_grid_y);

    #if 0
    if (!initialised_normal_buffer) {
	initialsed_normal_buffer = true;
	normal_buffer = (bool*)malloc(sizeof(bool) * texture_result.im_width * texture_result.im_height);
    }
    #endif
    
    SDL_Rect dest_rect = rect_init(dest_x, dest_y, scale*texture_result.im_width, scale*texture_result.im_height);

    SDL_SetRenderDrawColor(sdl_renderer, 0x0f, 0xff, 0xaf, 0xff);
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, texture_result.texture, NULL, &dest_rect);
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x00, 0x00, 0xff);

    //there was some rounding error here
    //until I switched to integer arithmetic like a BAWS
    for (uint32 line_x = 0; line_x < grid_lines_x; line_x++) {
	int32 pos_x = (grid_width * line_x) /grid_lines_x;
	SDL_RenderDrawLine(sdl_renderer, dest_x + pos_x, grid_start_y, dest_x + pos_x, grid_end_y);
    }

    for (uint32 line_y = 0; line_y < grid_lines_y; line_y++) {
	int32 pos_y = (grid_height*line_y)/grid_lines_y;

	SDL_RenderDrawLine(sdl_renderer, grid_start_x, dest_y + pos_y, grid_end_x, dest_y + pos_y);
    }

    
    
    SDL_RenderPresent(sdl_renderer);
    
}

