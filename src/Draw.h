#ifndef DRAW_H
#define DRAW_H

#include <iostream>
#include <Eigen/Core>

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

    static LineProgram *line_program = ProgramRegistry::getProgram<LineProgram>("line");
    assert(line_program);

    const Vertex vertices[2] = {
        { x0, y0, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f },
        { x1, y1, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f }
    };

    mat4x4 m, p, mvp;
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_identity(m);
    mat4x4_mul(mvp, p, m);

    line_program->bindVertexArray();
    line_program->use();
    line_program->set(mvp, 1.0, ratio);
    line_program->bufferData(sizeof(vertices), (void *)vertices);
    glDrawArrays(GL_LINES, 0, 2);
    line_program->unuse();
    line_program->unbindVertexArray();
}

//Eigen::Mat44 zz;
//using Eigen::Mat44;
inline void make_matrix(Mat44 &mat, float ratio, float x, float y, float angle, float xsize, float ysize) {
    mat4x4 m, p, mvp;
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 0.0f, 1.0f);
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, x, y, 0.0);
    mat4x4_rotate_Z(m, m, angle);
    mat4x4_scale_aniso(m, m, xsize, -ysize, 1.0f);
    mat4x4_mul(mvp, p, m);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat(i, j) = mvp[i][j];
        }
    }
}

#if 1
inline void drawFire(float ratio, float x, float y, float z, float angle, float xsize, float ysize, float brightness, GLuint tex) {
    Mat44 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    static FireProgram *fire_program = ProgramRegistry::getProgram<FireProgram>("fire");
    assert(fire_program);

    fire_program->use();
    fire_program->bindVertexArray();
    fire_program->set(mvp, now(), z);
    fire_program->bufferData(sizeof(vertices), (void *)vertices);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    fire_program->unbindVertexArray();
    fire_program->unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}
#endif

inline void drawShadow(float ratio, float x, float y, float z,
                       float angle, float xsize, float ysize, float alpha) {
    Mat44 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    static ShadowProgram *shadow_program = ProgramRegistry::getProgram<ShadowProgram>("shadow");
    assert(shadow_program);

    shadow_program->use();
    shadow_program->bindVertexArray();
    shadow_program->set(mvp, alpha, z);
    shadow_program->bufferData(sizeof(vertices), (void *)vertices);
    shadow_program->draw();
    shadow_program->unbindVertexArray();
    shadow_program->unuse();
}

inline void drawTextRectangle(float ratio, float x, float y, float angle, float xsize, float ysize, float brightness, GLuint tex) {
    Mat44 mvp;
    make_matrix(mvp, ratio, x, y, angle, xsize, ysize);

    static TextProgram *text_program = ProgramRegistry::getProgram<TextProgram>("text");
    assert(text_program);

    text_program->use();
    text_program->bindVertexArray();
    text_program->set(mvp, brightness, RGB {1.0, 1.0, 0.5});
    text_program->bufferData(sizeof(vertices), (void *)vertices);
    glBindTexture(GL_TEXTURE_2D, tex);
    text_program->draw();
    text_program->unbindVertexArray();
    text_program->unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
