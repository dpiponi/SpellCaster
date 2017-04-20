#ifndef SHADERS_H
#define SHADERS_H

#include <iostream>

using std::cout;
using std::endl;

#include <glad/glad.h>

#include "linmath.h"

#include "json11/json11.hpp"
using json11::Json;

GLuint compileProgram(Json shader);

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
    GLint mvp_location, brightness_location, vpos_location, vcol_location, uv_location, is_alpha_location, z_location;
public:
    Program () { } // XX Should go eventually
    Program(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        brightness_location = uniform("brightness");
        is_alpha_location = uniform("is_alpha");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        vcol_location = attrib("vCol", 3, 7, 2);
        uv_location = attrib("uvCoord", 2, 7, 5);

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float brightness, float is_alpha, float z) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(brightness_location, brightness);
        glUniform1f(is_alpha_location, is_alpha);
        glUniform1f(z_location, z);
    }
};

class ShadowProgram : public ProgramBase {
    GLint mvp_location, vpos_location, alpha_location, z_location;
public:
    ShadowProgram () { } // XX Should go eventually
    ShadowProgram(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        alpha_location = uniform("alpha");

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float alpha, float z) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(alpha_location, alpha);
        glUniform1f(z_location, z);
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

// XXX Duplicate
struct RGB {
    float r;
    float g;
    float b;
};

class TextProgram : public ProgramBase {
    GLint mvp_location, brightness_location, vpos_location, color_location, uv_location;
public:
    TextProgram () { } // XX Should go eventually
    TextProgram(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        brightness_location = uniform("brightness");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        color_location = uniform("color");

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float brightness, RGB rgb) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(brightness_location, brightness);
        glUniform3f(color_location, rgb.r, rgb.g, rgb.b);
    }
};

class FireProgram : public ProgramBase {
    GLint mvp_location, z_location, vpos_location, uv_location, time_location;
public:
    FireProgram () { } // XX Should go eventually
    FireProgram(Json shader) : ProgramBase(shader) {
        mvp_location = uniform("MVP");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        time_location = uniform("time");

        unbindVertexArray();
    }
    void set(const mat4x4 &mvp, float time, float z) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(time_location, time);
        glUniform1f(z_location, z);
    }
};

extern Program program;
extern ShadowProgram shadow_program;
extern LineProgram line_program;
extern TextProgram text_program;
extern FireProgram fire_program;

extern int width, height;

//extern GLint attribute_coord, uniform_tex;
#endif
