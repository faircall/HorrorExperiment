
#ifndef _GMO_MATH_H_

#include "gmo_types.h"
#include <math.h>
typedef struct {
    real32 x;
    real32 y;    
} vec2;

typedef struct {
    real32 x;
    real32 y;
    real32 z;    
} vec3;

#define PI 3.1415926

real32 scalar_distance(real32 x, real32 y);

vec2 vec2_init(real32 x, real32 y);
vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);

vec2 vec2_scale(real32 scalar, vec2 a);
real32 vec2_mag(vec2 a);
vec2 vec2_normalize(vec2 a);
real32 vec2_inner(vec2 a, vec2 b);
real32 vec2_angle(vec2 a, vec2 b);
real32 vec2_distance(vec2 a, vec2 b);
vec3 vec3_init(real32 x, real32 y, real32 z);

#define _GMO_MATH_H_
#endif
