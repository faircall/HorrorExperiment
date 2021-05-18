#ifndef _G_GAMEDEFS_H_

#include "SDL.h"
#include "gmo_types.h"
#include "gmo_math.h"
#include "sdl_util.h"
#include "gmo_sound.h"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

typedef enum {
    BRAIN_RESTING,
    BRAIN_TRACKING,
    BRAIN_ATTACKING,
    BRAIN_DYING,
    BRAIN_DEAD
} BRAIN_STATE;

typedef enum {
    GAME_TITLE,
    GAME_PLAYING,
    GAME_LAKE,
    GAME_FISHING,
    GAME_OVER,
    GAME_EDITOR //this doesn't quite belong here
} GAME_STATE;

typedef enum {
    EDITOR_COLLISION,
    EDITOR_NORMAL_MAP
} EDITOR_STATE;

typedef struct {
    int32 id;
    vec2 position;
    vec2 heading;
    vec2 last_heading;
    vec2 velocity;
    vec2 acceleration;
    real32 acceleration_value;
    real32 max_speed;
    bool attacking;
    //store info on arms, legs
    bool exists;
    //health etc
    real32 health;
    real32 attack_timer;
    SDL_Rect rect;
    BRAIN_STATE brain_state;
} Entity;

typedef struct {
    vec2 location;
    real32 timer;
    bool animation_playing;
    int32 animation_frame;
    int32 max_frames;
} WaterRipple;

typedef struct {
    GAME_STATE current_state;
    int32 current_score;
    int32 high_score;
    real32 dt;
    real32 total_time_ms;
    vec2 fish_location;
    WaterRipple *water_ripples;
} GameState;

typedef struct {
    SDL_Rect original_rect;
    SDL_Rect potential_rect;    
} RectToSave;

typedef struct {
    ALuint *sound_sources;
    ALuint *sound_buffers;
    TextureResult *textures;
} GameResource;



#define _G_GAMEDEFS_H_
#endif
