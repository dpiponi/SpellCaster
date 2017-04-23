#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <chrono>
#include <iostream>
#include <Eigen/Core>
#include <map>

using Eigen::Vector2f;

#include <glad/glad.h>

#include "Animated.h"
#include "shaders.h"
#include "Draw.h"

using std::cout;
using std::endl;
using std::map;
using std::shared_ptr;

typedef Vector2f Point;

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

class Drawable {
public:
    virtual void draw(float ratio, float border_line_width = 0.0) = 0;
    bool contains(Point point) const { return 0; }
};

class Rectangle : public Drawable {
protected:
    // Not happy about these mutables
    // but class does housekeeping in background.
    mutable Animated<float> angle;
    mutable Animated<float> x;
    mutable Animated<float> y;
    mutable Animated<float> z;
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
    float getZ() {
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
    void setPosition(double time) {
        x.addEvent(time);
        y.addEvent(time);
    }
    void setPosition(double time, double x0, double y0) {
        x.addEvent(time, x0);
        y.addEvent(time, y0);
    }
    void setZ(double time) {
        z.addEvent(time);
    }
    void setPosition(double time, Vector2f x0) {
        x.addEvent(time, x0[0]);
        y.addEvent(time, x0[1]);
    }
    void setZ(double time, double z0) {
        z.addEvent(time, z0);
    }
    void setSize(double time) {
        xsize.addEvent(time);
        ysize.addEvent(time);
    }
    void setSize(double time, double x0, double y0) {
        xsize.addEvent(time, x0);
        ysize.addEvent(time, y0);
    }
    void setBrightness(double time) {
        brightness.addEvent(time);
    }
    void setBrightness(double time, double b) {
        brightness.addEvent(time, b);
    }
    void setAngle(double time) {
        angle.addEvent(time);
    }
    void setAngle(double time, double a) {
        angle.addEvent(time, a);
    }

    void drawBorder(float ratio, float line_width);
    void drawShadow(float ratio);
    void draw(float ratio, float border_line_width = 0.0) override;
    //void drawText(float ratio);
    bool contains(Point point) const;
};

class TextRectangle : public Rectangle {
public:
    void draw(float ratio, float border_line_width = 0.0) override;
};

class Group : public Drawable {
    int next;
    map<int, shared_ptr<Drawable>> dictionary;
public:
    Group() : next(1) { }
    void draw(float ratio, float border_line_width = 0.0) override {
        for (auto p : dictionary) {
            cout << "DRAWING GROUP ELT" << endl;
            p.second->draw(ratio, border_line_width);
        }
    }
    int addElement(shared_ptr<Drawable> d) {
        int id = next++;
        dictionary[id] = d;
        return id;
    }
    void removeElement(int id) {
        dictionary.erase(id);
    }
};

#endif
