#ifndef SPELLCASTER_MATH_H
#define SPELLCASTER_MATH_H

#include "Types.h"

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
    return x-1;
}

inline float orientation(Vector2f v) {
    return atan2(-v[0], v[1]);
}

#endif
