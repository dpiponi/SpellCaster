#ifndef TEXT_RECTANGLE_H
#define TEXT_RECTANGLE_H

#include "Rectangle.h"

class TextRectangle : public Rectangle {
public:
    void drawz(float zlo, float zhi, float ratio, float border_line_width = 0.0) override;
};

class LinearGroup : public Drawable {
    vector<shared_ptr<TextRectangle>> characters;
public:
    LinearGroup() { }
    void drawz(float zlo, float zhi, float ratio, float border_line_width = 0.0) override {
        for (auto p : characters) {
            p->drawz(zlo, zhi, ratio, /* ignore1 */ 0.0);
        }
    }
    void reset() {
        characters.resize(0);
    }
    void appendElement(shared_ptr<TextRectangle> d) {
        characters.push_back(d);
    }
};

#endif
