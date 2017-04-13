#ifndef SHADERS_H
#define SHADERS_H

#include <iostream>

using std::cout;
using std::endl;

#include <glad/glad.h>

#include "linmath.h"

#include "json11/json11.hpp"
using json11::Json;

//extern const char *vertex_shader_src;
//extern const char *fragment_shader_src;
extern const char *vertex_shader_line;
extern const char *fragment_shader_line;
extern const char *vertex_shader_text;
extern const char *fragment_shader_text;
extern const char *vertex_shader_shadow;
extern const char *fragment_shader_shadow;

GLuint compileProgram(Json shader);

//extern GLuint vertex_buffer;
//extern GLuint line_vertex_buffer;

class ProgramBase {
protected:
    GLuint program;
    GLuint vao;
    GLuint vertex_buffer;
public:
    ProgramBase() { } // XXX Delete eventually.
    void bindVertexArray() {
        glBindVertexArray(vao);
    }
    void unbindVertexArray() {
        glBindVertexArray(0);
    }
    void use() {
        glUseProgram(program);
    }
    void unuse() {
        glUseProgram(0);
    }
    ProgramBase(Json shader) {
        program = compileProgram(shader);

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    }

    GLuint uniform(const char *uniform_name) {
        return glGetUniformLocation(program, uniform_name);
    }

    GLuint attrib(const char *attrib_name, int size, int stride, int offset) {
        GLuint location = glGetAttribLocation(program, attrib_name);
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE,
                              sizeof(float)*stride, (void *)(sizeof(float)*offset));
        return location;
    }
    void bufferData(int size, void *vertices) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // ?? 
    }
};

class Program : public ProgramBase {
    GLint mvp_location, brightness_location, vpos_location, vcol_location, uv_location, is_alpha_location;
public:
    Program () { } // XX Should go eventually
    Program(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        brightness_location = uniform("brightness");
        is_alpha_location = uniform("is_alpha");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        vcol_location = attrib("vCol", 3, 7, 2);
        uv_location = attrib("uvCoord", 2, 7, 5);

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float brightness, float is_alpha) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(brightness_location, brightness);
        glUniform1f(is_alpha_location, is_alpha);
    }
};

class ShadowProgram : public ProgramBase {
    GLint mvp_location, vpos_location;
public:
    ShadowProgram () { } // XX Should go eventually
    ShadowProgram(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    }
};

class LineProgram : public ProgramBase {
    GLint mvp_location, brightness_location, vpos_location, vcol_location, ratio_location;
public:
    LineProgram () { } // XX Should go eventually
    LineProgram(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        brightness_location = uniform("brightness");
        ratio_location = uniform("ratio");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        vcol_location = attrib("vCol", 3, 7, 2);

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float brightness, float ratio) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(brightness_location, brightness);
        glUniform1f(ratio_location, ratio);
    }
};

extern Program program;
extern ShadowProgram shadow_program;
extern LineProgram line_program;

extern int width, height;

//extern GLint attribute_coord, uniform_tex;
#endif
