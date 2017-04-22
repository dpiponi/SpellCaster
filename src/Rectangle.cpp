#include "GLError.h"
#include "Rectangle.h"

void Rectangle::drawShadow(float ratio, float is_alpha) {
    if (!visible || tex==0) {
        return;
    }

    ::drawShadow(ratio, x.get()-0.02, y.get()-0.02, z.get()-0.001, angle.get(), xsize.get(), ysize.get(),
                 /* alpha= */ 0.35);
}

void Rectangle::draw(float ratio, float border_line_width, float is_alpha) {
    if (!visible || tex==0) {
        return;
    }

    if (shadow) {
        drawShadow(ratio, is_alpha);
    }

    drawRectangle(ratio, x.get(), y.get(), z.get(), angle.get(), xsize.get(), ysize.get(), brightness.get(), is_alpha, tex);

    drawBorder(ratio, border_line_width);
}

bool Rectangle::contains(Point point) const {
    if (!visible) {
        return false;
    }

    float ratio = width / (float) height;
    mat4x4 m, p, mvp, inv;
    vec4 q, r;

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, x.get(), y.get(), 0.0);
    mat4x4_rotate_Z(m, m, angle.get());
    mat4x4_scale_aniso(m, m, xsize.get(), -ysize.get(), 1.0f);
    mat4x4_mul(mvp, p, m);
    mat4x4_invert(inv, mvp);

    r[0] = 2*point[0]/width-1;
    r[1] = 2*(height-1-point[1])/height-1;
    r[2] = 0.0;
    r[3] = 1.0;
    mat4x4_mul_vec4(q, inv, r);

    // Replace division with multiplication as soon as sure of sign of q[3]
    float u = q[0]/q[3];
    float v = q[1]/q[3];
    return -1 <= u && u <= 1 && -1 <= v && v <= 1;
}

void Rectangle::drawBorder(float ratio, float line_width) {
    double x0 = x.get()-xsize.get();
    double x1 = x.get()+xsize.get();
    double y0 = y.get()-ysize.get();
    double y1 = y.get()+ysize.get();
    if (highlight) {
        drawLine(ratio, line_width, RGB {r_highlight, g_highlight, b_highlight}, x0, y0, x1, y0);
        drawLine(ratio, line_width, RGB {r_highlight, g_highlight, b_highlight}, x1, y0, x1, y1);
        drawLine(ratio, line_width, RGB {r_highlight, g_highlight, b_highlight}, x1, y1, x0, y1);
        drawLine(ratio, line_width, RGB {r_highlight, g_highlight, b_highlight}, x0, y1, x0, y0);
    }
}

void TextRectangle::draw(float ratio, float ignore1, float ignore2) {
    if (!visible || tex==0) {
        return;
    }

    drawTextRectangle(ratio, x.get(), y.get(), angle.get(), xsize.get(), ysize.get(), brightness.get(), tex);
}
