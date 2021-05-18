#include "gmo_math.h"

real32 scalar_distance(real32 x, real32 y)
{
    real32 result = sqrt((x - y)*(x - y));
    return result;
}

vec2 vec2_init(real32 x, real32 y)
{
    vec2 result;
    result.x = x;
    result.y = y;
    return result;
}

vec2 vec2_add(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;    
    return result;
}

vec2 vec2_sub(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

vec2 vec2_scale(real32 scalar, vec2 a)
{
    vec2 result;
    result.x = scalar*a.x;
    result.y = scalar*a.y;
    return result;
}

real32 vec2_mag(vec2 a)    
{
    real32 result = sqrt(a.x*a.x + a.y*a.y);
    return result;
}

vec2 vec2_normalize(vec2 a)
{
    vec2 result;
    result.x = 1.0f;
    result.y = 0.0f;
    real32 mag = vec2_mag(a);
    //maybe an epsilon is needed
    if (mag != 0.0f) {
	result.x = a.x/mag;
	result.y = a.y/mag;
    }
    return result;
}

real32 vec2_inner(vec2 a, vec2 b)
{
    real32 result = a.x*b.x + a.y*b.y;
    return result;
}

real32 vec2_angle(vec2 a, vec2 b)
{
    real32 result;
    real32 inner = vec2_inner(a, b);
    if (inner == 0.0f) {
	result = PI/2.0f; 
    } else {
	result = acos(inner / (vec2_mag(a) * vec2_mag(b)));
    }
    return result;
}

real32 vec2_distance(vec2 a, vec2 b)
{
    real32 result;
    vec2 distance_vec = vec2_sub(a, b);
    result = vec2_mag(distance_vec);
    return result;
}

vec3 vec3_init(real32 x, real32 y, real32 z)
{
    vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
