#ifndef SPELLCASTER_MATH_H
#define SPELLCASTER_MATH_H

#include "Types.h"

float clamp(float x, float a, float b) {
    if (a < b) {
        return x < a ? a : (x > b ? b : x);
    }
    if (b < a) {
        return x < b ? b : (x > a ? a : x);
    }
    return a;
}

float linstep(float a, float b, float x) {
    return clamp((x-a)/(b-a), 0.0, 1.0);
}

float smoothstep(float edge0, float edge1, float x) {
    x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0); 
    return x*x*(3 - 2*x);
}

static float triangle(float x) {
    if (x < -1) {
        return 0.0;
    }
    if (x > 1) {
        return 0.0;
    }
    if (x > 0) {
        return 1-x;
    }
    return x+1;
}

inline float orientation(Vector2f v) {
    return atan2(-v[0], v[1]);
}

#endif
