#ifndef _E_EDITOR_H_
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tchar.h>

#include "SDL.h"
#include "gmo_types.h"
#include "g_gameutils.h"
#include "g_gamedefs.h"


void collision_update_and_render(SDL_Renderer *sdl_renderer, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv, EDITOR_STATE *editor_state, bool *want_save, bool *want_load);

void normal_map_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, bool *g_running, EDITOR_STATE *editor_state);

void editor_update_and_render(SDL_Renderer *sdl_renderer, GameResource *game_resources, SDL_Rect **editor_rect_list, uint32 *num_rects, uint32 *rect_storage, int32 *mouse_x, int32 *mouse_y, bool *g_running, char *argv);    

#define _E_EDITOR_H_
#endif
