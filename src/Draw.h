#ifndef DRAW_H
#define DRAW_H

#include <iostream>

using std::cout;
using std::endl;

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Animated.h"
#include "shaders.h"
#include "geometry.h"
#include "linmath.h"

#if 0
struct RGB {
    float r;
    float g;
    float b;
};
#endif

//extern GLuint text_vertex_buffer;

inline void drawLine(float ratio, float line_width, RGB rgb,
                     float x0, float y0, float x1, float y1) {
    const Vertex vertices[2] = {
        { x0, y0, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f },
        { x1, y1, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f }
    };

    mat4x4 m, p, mvp;
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_identity(m);
    mat4x4_mul(mvp, p, m);

    line_program.bindVertexArray();
    line_program.use();
    line_program.set(mvp, 1.0, ratio);
    line_program.bufferData(sizeof(vertices), (void *)vertices);
    glDrawArrays(GL_LINES, 0, 2);
    line_program.unuse();
    line_program.unbindVertexArray();
}

inline void make_matrix(mat4x4 mvp, float ratio, float x, float y, float angle, float xsize, float ysize) {
    mat4x4 m, p;
    //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 0.0f, 1.0f);
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, x, y, 0.0);
    mat4x4_rotate_Z(m, m, angle);
    mat4x4_scale_aniso(m, m, xsize, -ysize, 1.0f);
    mat4x4_mul(mvp, p, m);
}

#if 1
inline void drawFire(float ratio, float x, float y, float z, float angle, float xsize, float ysize, float brightness, float is_alpha, GLuint tex) {
    mat4x4 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    fire_program.use();
    fire_program.bindVertexArray();
    fire_program.set(mvp, now(), z);
    fire_program.bufferData(sizeof(vertices), (void *)vertices);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    fire_program.unbindVertexArray();
    fire_program.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline void drawRectangle(float ratio, float x, float y, float z, float angle, float xsize, float ysize, float brightness, float is_alpha, GLuint tex) {
    mat4x4 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    extern GLuint fire_tex;

    if (tex==fire_tex) {
        drawFire(ratio, x, y, z, angle, xsize, ysize, brightness, is_alpha, tex);
    }
    program.use();
    program.bindVertexArray();
    program.set(mvp, brightness, is_alpha, z);
    program.bufferData(sizeof(vertices), (void *)vertices);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.unbindVertexArray();
    program.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}
#endif

inline void drawShadow(float ratio, float x, float y, float z,
                       float angle, float xsize, float ysize, float alpha) {
    mat4x4 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    shadow_program.use();
    shadow_program.bindVertexArray();
    shadow_program.set(mvp, alpha, z);
    shadow_program.bufferData(sizeof(vertices), (void *)vertices);
    shadow_program.draw();
    shadow_program.unbindVertexArray();
    program.unuse();
}

inline void drawTextRectangle(float ratio, float x, float y, float angle, float xsize, float ysize, float brightness, GLuint tex) {
    mat4x4 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    text_program.use();
    text_program.bindVertexArray();
    text_program.set(mvp, brightness, RGB {1.0, 1.0, 0.5});
    text_program.bufferData(sizeof(vertices), (void *)vertices);
    glBindTexture(GL_TEXTURE_2D, tex);
    text_program.draw();
    text_program.unbindVertexArray();
    text_program.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
