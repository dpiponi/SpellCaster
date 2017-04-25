#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "Types.h"

class Drawable {
public:
    virtual void drawz(float zlo, float zhi, float ratio, float border_line_width = 0.0) = 0;
    bool contains(Point point) const { return 0; }
};

#endif
