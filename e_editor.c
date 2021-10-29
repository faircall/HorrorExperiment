#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "e_editor.h"

/**
 * TODO:
 *
 * Fix the vec2_to_vec3 function
 * 
 * Could add support for polygons rather than
 * just rects
 *
 *
 * how to do fill-polygon region? try scanline YESSSSS
 *
 * so the problem here is I've inserted them in order of appearance,
 * rather than by co-ordinates.
 * 
 * step one- toggle between buffers
 * then have option to fill
 *
 * make a new struct to hold selection BUFFERS to track the count also
 *
 * TODO:
 * 
 * 
 * TODO:
 * 
 * also clean up the ui for how you have to select each selection etc
 * 
 * */



typedef struct {
    int x;
    int y;
    bool32 active;
} Mouse_Grid_Result;


//could add a flag here for 'closed' or not
typedef struct {
    Mouse_Grid_Result *buffer;//this is more like endpoints
    int count;
    Vec3 vec3_value;
} Selection_Buffer;

uint32 vec3_to_int32(Vec3 a);

Mouse_Grid_Result get_mouse_grid(int mouse_x, int mouse_y, uint grid_width, uint grid_height);

void scanline_fill(Mouse_Grid_Result *selection_buffer, int width, int height);

void clear_selection_buffer(Mouse_Grid_Result *buffer, int width, int height);

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, int32 *mouse_x, int32 *mouse_y, bool *g_running, EDITOR_STATE *editor_state);

void collision_update_and_render(SDL_Renderer *sdl_renderer, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv, EDITOR_STATE *editor_state, bool *want_save, bool *want_load);

void save_normals(Selection_Buffer *selection_buffers, int selection_buffers_count, int width, int height);

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
	result.x = -1;
	result.y = -1;
    } else {
    
	//we actually want to grid spacing...also
	result.x = (mouse_x - grid_start_x) / grid_spacing_x;
	result.y = (mouse_y - grid_start_y) / grid_spacing_y;
    }

    
    return result;
}

void scanline_fill(Mouse_Grid_Result *buffer, int width, int height)
{
    
    //step one get y-min y-max

    //we need to check if the selection buffer is fillable, i.e a closed polygon shape

    //does every cell have at least 2 neighbouring?
    for (int i = 0; i < height; i++) {
	int x_min = 0;
	int y_min = 0;
	int x_max = 0;
	int y_max = 0;
	Mouse_Grid_Result start, end;
	bool32 found_start = 0;
	bool32 found_end = 0;
	for (int j = 0; j < width; j++) {
	    Mouse_Grid_Result current = buffer[i * width + j];
	    if (current.active && !found_start) {
		start = current;
		found_start = 1;
		continue;
	    }
	    if (current.active && found_start) {
		end = current;
		found_end = 1;
		continue;
	    }
	}
	if (found_start && found_end) {
	    //fill if we have a start and end
	    for (int j = start.x; j < end.x; j++) {
		Mouse_Grid_Result add = {.x = j, .y = i, .active = 1};
		buffer[i * width + j] = add;
	    }
	}
    }
}

Vec3 vec2_to_vec3_normal(Vec2 vec2_normal)
{
    Vec3 result = {0.0f, 0.0f, 1.0f};
    //start off as a vertical line straight up

    float mag = vec2_mag(vec2_normal);
    
    float x_comp = vec2_normal.x;
    float y_comp = vec2_normal.y;
    float z_comp = 100.0f - mag;//very suspicious

    result.x = x_comp;
    result.y = y_comp;
    result.z = z_comp;

    result = vec3_normalize(result);
    
    printf("calculated vec3 %f %f %f \n", result.x, result.y, result.z);
    
    
    return result;
}

uint32 vec3_to_int32(Vec3 a)
{

    //don't think this is working right
    real32 red_real = 127.5 + a.x*127.5;
    real32 green_real = 127.5 + a.y*127.5;
    real32 blue_real = 127.5 + a.z*127.5;

    uint32 result = 0x000000ff;
    
    uint32 r = (uint32)red_real;
    uint32 g = (uint32)green_real;
    uint32 b = (uint32)blue_real;//this is not working right

    printf("the real values were %f %f %f\n",  red_real, green_real, blue_real);

    printf("just to check, before we have red: %d, green: %d, blue: %d values \n", r, g, b);


    //this might still be wrong?
    r = r << 6*4;//note that it shifts bits, not bytes! One hex "bit" (hit?) is 4 bits
    g = g << 4*4;//is it?
    b = b << 2*4;

    //what?
    printf("and after we have red: %d, green: %d, blue: %d values \n", r, g, b);

    result = result + r + g + b;
    //why is everything always yellow?
    return result;

}

void save_normals(Selection_Buffer *selection_buffers, int selection_buffers_count, int width, int height)
{
    char file_name_buffer[50];
    get_string_stripped(file_name_buffer, 50);
    char *appended_png;
    appended_png = string_concatenate(file_name_buffer, ".png");

    //we actually want to write a .png file here which is the normal map I believe, to play well with the pixel shader
    //to do that, we first need to set aside a buffer
    //for the dimensions of the .png
    //it will be rgba (or rgb?)
    //then we populate with int32s
    //which are packed bytes with the 0-255 value of r,g,b
    //and THAT comes from the normal component x, y, z from mapped from -1.0 to 1.0
    //to 0-255

    //to do that mapping....we could...
    //want -1->0, 1->255
    //0.5 would be.... 128?
    //first add 1 so we have 0-2
    //then do.... 127.5 + 127.5*component I think will work
    //then round to nearest int

    int32 *image_buffer = (int32*)malloc(width * height * sizeof(int32));

    for (int i = 0; i < width; i++) {
	for (int j = 0; j < height; j++) {
	    image_buffer[j*width + i] = 0xffffffff;
	}
    }

    for (int i = 0; i < selection_buffers_count; i++) {
	Selection_Buffer current_selection_buffer = selection_buffers[i];

	//this isn't packed right on blue
	int32 packed_normal_value = vec3_to_int32(current_selection_buffer.vec3_value);

	for (int row = 0; row < height; row++) {
	    for (int col = 0; col < width; col++) {
		Mouse_Grid_Result current_grid = current_selection_buffer.buffer[row * width + col];
		if (current_grid.active) {
		    image_buffer[row*width + col] = packed_normal_value;
		}
	    }
	}
	
	for (int j = 0; j < current_selection_buffer.count; j++) {
	    //different now
	    Mouse_Grid_Result current_grid = current_selection_buffer.buffer[j];	    
	    image_buffer[current_grid.y*width + current_grid.x] = packed_normal_value;
	}
    }
    int image_write_result = stbi_write_png(appended_png, width, height, 4, (void*)image_buffer, width*4);

    if (!image_write_result) {
	printf("failed to write the png file\n");
    } else {
	printf("apparnetly we wrote the file? where is it?\n");
    }
	   

    free(image_buffer);
    //now to write this image buffer
    //use stb_image_write

}

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, int32 *mouse_x, int32 *mouse_y, bool *g_running, EDITOR_STATE *editor_state)
{
    //a VERY nice side effect
    //of using static variables here is that
    //the persist even when we switch states (e.g to collision).
    //of course, the downside is we use more memory (specifically in the data segment)
    //so we should eventually switch all this over to pre-allocated memory

    //todo: able to delete a selection
    //todo: be able to add more than one selection
    //something is not working correctly here
    
    SDL_Event event;
    static int texture_counter = 0;

    static bool selecting = false;

    static Mouse_Grid_Result selecting_start = {0, 0};
    
    static int selection_buffers_count = 0;
    static int selection_buffers_size = 32;
    static Selection_Buffer *selection_buffers = NULL;
    static bool initialized_selection_buffers = false;
    static int buffer_counter = 0;

    static bool started_normal = false;

    static Vec2 normal_start = {0.0f, 0.0f};
    static Vec2 normal_end = {0.0f, 0.0f};

    static Vec2 normal_to_draw = {0.0f, 0.0f};
	    

    //static 

    if (!initialized_selection_buffers) {
	initialized_selection_buffers = true;
	selection_buffers = (Selection_Buffer*)malloc(selection_buffers_size * sizeof(Selection_Buffer));
    }
    

    //we could make a static pointer here
    //try it out for size 
    //static bool *normal_map_buffer = (bool*)malloc(sizeof());;

        //texture_counter = texture_counter % NUM_TEX;

    TextureResult texture_result = game_resources->textures[texture_counter];
    //get grid-line info to render grid lines
    int32 scale = 4;
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

    bool want_save = false;

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
	    if (event.key.keysym.scancode == SDL_SCANCODE_B) {
		buffer_counter++;
		if (buffer_counter >= selection_buffers_count) {
		    buffer_counter = 0;
		}
		Vec3 vec3_current_selection = selection_buffers[buffer_counter].vec3_value;
		printf("current selection (%d) has x: %f y: %f z: %f \n", buffer_counter, vec3_current_selection.x, vec3_current_selection.y, vec3_current_selection.z);
	    }

	    if (event.key.keysym.scancode == SDL_SCANCODE_D) {
		//handle deletions
		//could be a bit tricky, need to shift things around!
		
	    }

	    if (event.key.keysym.scancode == SDL_SCANCODE_F) {
		//fill!
		scanline_fill(selection_buffers[buffer_counter].buffer, texture_result.im_width, texture_result.im_height);
	    }

	    if (event.key.keysym.scancode == SDL_SCANCODE_S) {
		want_save = true;
		save_normals(selection_buffers, selection_buffers_count, texture_result.im_width, texture_result.im_height);
		//for (int i = 0; i < selection_buffers_count; i++) {
		//    //vec3 current_vec3 = selection_buffers[i].vec3_value;
		//    save_normals(selection_buffers, selection_buffers_count, texture_result.im_width, texture_result.im_height);
		    //int32 packed_pixel = vec3_to_int32(current_vec3);
		//}
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
		    //actually want this to be a CELL not a vec
		    selecting_start = get_mouse_grid(*mouse_x, *mouse_y, scale, scale, grid_start_x, grid_start_y, grid_end_x, grid_end_y);
		    selecting_start.active = 1;
		    selection_buffers[selection_buffers_count].count = 0;
		    selection_buffers[selection_buffers_count].vec3_value = vec3_init(0.0f, 0.0f, 0.0f);
		    selection_buffers[selection_buffers_count].buffer = (Mouse_Grid_Result*)malloc(sizeof(Mouse_Grid_Result) * texture_result.im_width * texture_result.im_height);
		    //init it to 'empty' results
		    clear_selection_buffer(selection_buffers[selection_buffers_count].buffer, texture_result.im_width, texture_result.im_height);
		    selection_buffers[selection_buffers_count].buffer[texture_result.im_width * selecting_start.y + selecting_start.x] = selecting_start;
		    selection_buffers[selection_buffers_count].count++;

		} else {
		    //why is this not working every time?
		    //it's due to the realloc....
		    //ie it happens after 2, 4, 8, 16 times etc
		    //selection_buffers[selection_buffers_count] = (Selection_Buffer*)malloc(texture_result.im_width * texture_result.im_height * sizeof(Selection_Buffer));
		    //memcpy(selection_buffers[selection_buffers_count], selection_buffer, texture_result.im_width * texture_result.im_height * sizeof(Selection_Buffer));
		    selection_buffers_count++;
		    //resize if necessary
		    if (selection_buffers_count > selection_buffers_size) {

			//we are losing something here
			//Selection_Buffer *temp_buffer = (Selection_Buffer*)malloc(sizeof(Selection_Buffer) * selection_buffers_size);

			//memcpy((void*)temp_buffer, (void*)selection_buffers, selection_buffers_count * sizeof(Selection_Buffer));
			
			//free(selection_buffers);
			
			//selection_buffers = NULL;
			Selection_Buffer most_recent = selection_buffers[selection_buffers_count-1];
			
			selection_buffers = (Selection_Buffer*)realloc(selection_buffers, selection_buffers_size * 2 * sizeof(Selection_Buffer));
			selection_buffers[selection_buffers_count-1] = most_recent;
			//selection_buffers[selection_buffers_count]

			//memcpy((void*)selection_buffers, (void*)temp_buffer, selection_buffers_count * sizeof(Selection_Buffer));
			
			//free(temp_buffer);

			selection_buffers_size *= 2;
		    }
		    //then clean up
		    
		    //free(selection_buffer);
		    //selection_buffer = NULL;
		    selecting = false;
		}
	    }
	    else if (event.button.button == SDL_BUTTON_RIGHT) {
		//make a normal vector
		if (!started_normal) {
		    normal_start.x = *mouse_x;
		    normal_start.y = *mouse_y;
		    started_normal = true;
		    
		} else {
		    started_normal = false;

		    //convert the vec2 to a vec3
		    printf("normal to draw has magnitude %f\n", vec2_mag(normal_to_draw));
		    Vec3 normal_to_add = vec2_to_vec3_normal(normal_to_draw);
		    //printf("normal to add was %f %f %f\n", normal_to_add.x, normal_to_add.y, normal_to_add.z);
		    
		    //somehow we are only writing this the first time through
		    selection_buffers[buffer_counter].vec3_value = normal_to_add;
		    //selection_buffers[selection_buffers_count].vec3_value = normal_to_add;
		}
		
		
	    }
	}
    }

    

    //need some kind of a select region function

    //how to select a particular cell??

    
    

    if (texture_counter >= NUM_TEX) {
	texture_counter = 0;
    }
    if (texture_counter < 0) {
	texture_counter = NUM_TEX - 1;
    }


    int mouse_grid_x, mouse_grid_y;

    Mouse_Grid_Result grid_result = get_mouse_grid(*mouse_x, *mouse_y, scale, scale, grid_start_x, grid_start_y, grid_end_x, grid_end_y);
    mouse_grid_x = grid_result.x;
    mouse_grid_y = grid_result.y;

    if (selecting) {
	if (grid_result.x != selecting_start.x || grid_result.y != selecting_start.y) {
	    selecting_start = grid_result;
	    selecting_start.active = 1;
	    selection_buffers[selection_buffers_count].buffer[selecting_start.y * texture_result.im_width + selecting_start.x] = selecting_start;
	    selection_buffers[selection_buffers_count].count++;
	    if (selection_buffers[selection_buffers_count].count>= texture_result.im_width * texture_result.im_height) {
		selection_buffers[selection_buffers_count].count= texture_result.im_width * texture_result.im_height; //prevent buffer overflow, though handle this better
	    }
	}
    }
    
    //printf("grid x %d, grid y %d \n", mouse_grid_x, mouse_grid_y);

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

    //why check this?
    if (mouse_grid_x != -1 && mouse_grid_y != -1) {
	SDL_Rect normal_cell = rect_init(grid_start_x + mouse_grid_x*scale, grid_start_y + mouse_grid_y*scale, scale, scale);
	SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0x00, 0x10);
	SDL_RenderDrawRect(sdl_renderer, &normal_cell);
    }

#if 1
    if (selecting) {
	SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0xff, 0x10);
	Selection_Buffer selection_buffer = selection_buffers[selection_buffers_count];
	for (int i = 0; i < texture_result.im_width * texture_result.im_height; i++) {
	    SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0x00, 0xff, 0x10);
	    SDL_Rect normal_cell = rect_init(grid_start_x + selection_buffer.buffer[i].x*scale, grid_start_y + selection_buffer.buffer[i].y*scale, scale, scale);
	    if (selection_buffer.buffer[i].active) {
		SDL_RenderDrawRect(sdl_renderer, &normal_cell);
	    }
	    
	}

    }
#endif
    //printf("buffer counter is %d", buffer_counter);
    for (int i = 0; i < selection_buffers_count; i++) {
	if (i == buffer_counter) {
	    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xff, 0xff, 0x10);
	} else {
	    //draw the color as a normal
	    float color_scale = (float)i / (float)selection_buffers_count;
	    int red = 255 * color_scale;
	    SDL_SetRenderDrawColor(sdl_renderer, red, 0xff, 0x00, 0x10);
	}
	//we should do this with row/col representation
	for (int j = 0; j < texture_result.im_width * texture_result.im_height; j++) {

	    SDL_Rect normal_cell = rect_init(grid_start_x + selection_buffers[i].buffer[j].x*scale, grid_start_y + selection_buffers[i].buffer[j].y*scale, scale, scale);
	    //SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0xff, 0x00, 0x10);

	    if (selection_buffers[i].buffer[j].active) {
		SDL_RenderDrawRect(sdl_renderer, &normal_cell);
	    }
	}
    }

    if (started_normal) {
	normal_end.x = *mouse_x;
	normal_end.y = *mouse_y;
	normal_to_draw = vec2_sub(normal_end, normal_start);
	if (vec2_mag(normal_to_draw) >= 100.0f) {	    
	    normal_to_draw = vec2_normalize(normal_to_draw);
	    normal_to_draw = vec2_scale(normal_to_draw, 100.0f);
	}
	//the 3d normal will be z of 1 - magnitude of the vector,
	//x will be magntiude * x comp
	//y will be magnitude * y comp
	// but then 'normalized'
	//printf("normal to draw x, y: %f %f\n", normal_to_draw.x, normal_to_draw.y);
	SDL_SetRenderDrawColor(sdl_renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderDrawLine(sdl_renderer, normal_start.x, normal_start.y, normal_start.x + normal_to_draw.x, normal_start.y + normal_to_draw.y);
	
    }


    
    
    SDL_RenderPresent(sdl_renderer);
    
}

void clear_selection_buffer(Mouse_Grid_Result *buffer, int width, int height)
{
    for (int i = 0; i < height; i++) {
	for (int j= 0; j < width; j++) {
	    Mouse_Grid_Result result = {.x = 0, .y= 0, .active = 0};
	    buffer[i * width + j] = result;
	}
    }
}

