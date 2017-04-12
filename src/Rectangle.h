#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <chrono>
#include <iostream>

#include <glad/glad.h>

#include "Animated.h"
#include "shaders.h"
#include "Draw.h"

using std::cout;
using std::endl;

struct Point {
    float x;
    float y;
};

class Rectangle {
    // Not happy about these mutables
    // but class does housekeeping in background.
    mutable Animated<float> angle;
    mutable Animated<float> x;
    mutable Animated<float> y;
    mutable Animated<float> xsize;
    mutable Animated<float> ysize;
    mutable Animated<float> brightness;
    GLuint tex;
    float r_highlight;
    float g_highlight;
    float b_highlight;
    bool highlight;
public:
    bool shadow;
    bool visible;
    float getX() {
        return x.get();
    }
    float getY() {
        return y.get();
    }
    float getXSize() {
        return xsize.get();
    }
    float getYSize() {
        return ysize.get();
    }
    void setHighlight(RGB colour) {
        r_highlight = colour.r;
        g_highlight = colour.g;
        b_highlight = colour.b;
        highlight = true;
    }
    void setNoHighlight() {
        highlight = false;
    }
    Rectangle() : highlight(false), visible(false), tex(0), shadow(false) { }
    void setTexture(GLuint t) { tex = t; }
    void setPosition(double time, double x0, double y0) {
        x.addEvent(time, x0);
        y.addEvent(time, y0);
    }
    void setSize(double time, double x0, double y0) {
        xsize.addEvent(time, x0);
        ysize.addEvent(time, y0);
    }
    void setBrightness(double time, double b) {
        brightness.addEvent(time, b);
    }
    void setAngle(double time, double a) {
        angle.addEvent(time, a);
    }

    void drawBorder(float ratio, float line_width);
    void drawShadow(float ratio, float is_alpha);
    void draw(float ratio, float border_line_width, float is_alpha);
    bool contains(Point point) const;
};

#endif
