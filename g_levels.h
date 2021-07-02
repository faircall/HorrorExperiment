#ifndef _G_LEVELS_H_

#include <stdbool.h>
#include <stdlib.h>
#include <tchar.h>
#include "SDL.h"
#include "gmo_types.h"
#include "gmo_render.h"
#include "g_gamedefs.h"
#include "g_gameutils.h"

void do_title(GameState *game_state, GlobalRenderer global_renderer, GameResource game_resources, bool *g_running);

void do_lake(GameState *game_state, GlobalRenderer global_renderer, GameResource game_resources, bool *g_running);

void set_rod_rect(int32 mouse_x, int32 mouse_y, SDL_Rect *rect, int32 draw_rod_mid);


void do_fishing(GameState *game_state, GlobalRenderer global_renderer, GameResource game_resources, bool *g_running);

#define _G_LEVELS_H_
#endif
