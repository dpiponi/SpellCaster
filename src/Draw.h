#ifndef DRAW_H
#define DRAW_H

#include <iostream>

using std::cout;
using std::endl;

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shaders.h"
#include "geometry.h"
#include "linmath.h"

struct RGB {
    float r;
    float g;
    float b;
};

extern GLuint vertex_buffer, line_vertex_buffer, text_vertex_buffer;;

inline void drawLine(float ratio, float line_width, RGB rgb,
                     float x0, float y0, float x1, float y1) {
    const Vertex vertices[2] = {
        { x0, y0, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f },
        { x1, y1, rgb.r, rgb.g, rgb.b, 0.5f, 0.5f }
    };

    connect_line_shader();
    mat4x4 m, p, mvp;

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

    mat4x4_identity(m);

    mat4x4_mul(mvp, p, m);
//        glBindVertexArray(line_vertex_buffer);
    glUseProgram(line_program);
    glUniformMatrix4fv(line_mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniform1f(line_brightness_location, 1.0);
    glUniform1f(line_ratio_location, ratio);
    glBindBuffer(GL_ARRAY_BUFFER, line_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindTexture(GL_TEXTURE_2D, 0);
    //cout << "wi=" << line_width << endl;
    //glLineWidth(line_width);
    glDrawArrays(GL_LINES, 0, 2);
}

#if 0
extern FT_Face face;
inline void render_text(const char *text, float x, float y, float sx, float sy) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniform_tex, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint vbo; //XXX Temp1
    glGenBuffers(1, &vbo);
    cout << vbo << endl;
    //glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(attribute_coord);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

  FT_GlyphSlot g = face->glyph;
  const char *p;

  for(p = text; *p; p++) {
    if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
        continue;
 
	std::cout << g->bitmap.width << ' ' << g->bitmap.rows << std::endl;

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );
 
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;
 
    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };
 
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 
    x += (g->advance.x/64) * sx;
    y += (g->advance.y/64) * sy;
  }
}
#endif

#endif
