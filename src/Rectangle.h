#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <chrono>
#include <iostream>
#include <Eigen/Core>
#include <map>

#include <glad/glad.h>

#include "Drawable.h"
#include "Animated.h"
#include "shaders.h"
#include "Draw.h"

using std::cout;
using std::endl;
using std::map;
using std::shared_ptr;
using std::vector;

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
    mutable Animated<float> alpha;
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
    void setApha(double time) {
        alpha.addEvent(time);
    }
    void setAlpha(double time, double b) {
        alpha.addEvent(time, b);
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
    bool contains(Point point) const;
};

class Group : public Drawable {
    int next;
    map<int, shared_ptr<Drawable>> dictionary;
public:
    Group() : next(1) { }
    void draw(float ratio, float border_line_width = 0.0) override {
        for (auto p : dictionary) {
            p.second->draw(ratio, border_line_width);
        }
    }
    int addElement(shared_ptr<Drawable> d) {
        int id = next++;
        dictionary[id] = d;
        return id;
    }

    shared_ptr<Drawable> &getElement(int id) {
        return dictionary[id];
    }

    void removeElement(int id) {
        dictionary.erase(id);
    }
};

#endif
