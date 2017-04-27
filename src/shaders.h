#ifndef SHADERS_H
#define SHADERS_H

#include <iostream>
#include <map>
#include <string>
#include <Eigen/Core>

using Eigen::Matrix;
using Eigen::Vector3f;
typedef Matrix<float, 4, 4, Eigen::RowMajor> Mat44;

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::string;

#include <glad/glad.h>

#include "linmath.h"

#include "json11/json11.hpp"
//using json11::Json;

//GLuint compileProgram(Json shader);
GLuint compileProgram(const char *vertex_program, const char *fragment_program, const char *geometry_program = 0);

class ProgramBase;

class ProgramRegistry {
public:
    static map<string, ProgramBase *> *getRegistry() {
        cout << "Creating program registry" << endl;
        static map<string, ProgramBase *> *registry = new map<string, ProgramBase *>;
        return registry;
    }
    static void registerProgram(string name, ProgramBase *definition) {
        auto registry = getRegistry();
        (*registry)[name] = definition;
        cout << "Registering program: " << name << endl;
    }
    template<typename P> static P *getProgram(const char *name) {
        auto registry = getRegistry();
        auto p = registry->find(name);
        if (p == registry->end()) {
            cerr << "Couldn't find program " << name << endl;
        }
        P *program = dynamic_cast<P *>(p->second);
        assert(program);
        return program;
    }
    static void init();
};

class ProgramBase {
protected:
    GLuint program;
    GLuint vao;
    GLuint vertex_buffer;
    const char *vertex_program;
    const char *fragment_program;
    const char *geometry_program;
public:
    //ProgramBase() { } // XXX Delete eventually.
    void bindVertexArray() const {
        glBindVertexArray(vao);
    }
    void unbindVertexArray() const {
        glBindVertexArray(0);
    }
    void use() const {
        glUseProgram(program);
    }
    void unuse() const {
        glUseProgram(0);
    }
    ProgramBase(const char *name, const char *vertex_program0, const char *fragment_program0, const char *geometry_program0) :
        vertex_program(vertex_program0), fragment_program(fragment_program0), geometry_program(geometry_program0) {
        ProgramRegistry::registerProgram(name, this);
    }
    virtual void init() {
        cout << "compiling" << endl;
        program = compileProgram(vertex_program, fragment_program, geometry_program);
        cout << "compiling done" << endl;
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
    virtual ~ProgramBase() { /* dummy */ }
};

class RectangleProgram : public ProgramBase {
public:
    //RectangleProgram() { }
    RectangleProgram(const char *name, const char *vertex_program, const char *fragment_program, const char *geometry_program) : ProgramBase(name, vertex_program, fragment_program, geometry_program) { }
    virtual void set(const Mat44 &mvp, float alpha, float z, float time) = 0;
};

class Program : public RectangleProgram {
    GLint mvp_location, alpha_location, vpos_location, uv_location, z_location;
public:
    //Program() { } // XX Should go eventually
    Program(const char *name, const char *vertex_program, const char *fragment_program, const char *geometry_program = 0) : RectangleProgram(name, vertex_program, fragment_program, geometry_program) {
    }
    void init() override {
        ProgramBase::init();
        mvp_location = uniform("MVP");
        alpha_location = uniform("alpha");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        unbindVertexArray();
    }
    void set(const Mat44 &mvp, float alpha, float z, float time) override {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(alpha_location, alpha);
        glUniform1f(z_location, z);
    }
};

// XXX Duplicate
struct RGB {
    float r;
    float g;
    float b;
};

#if 0
extern Program program;
extern GlowProgram glow_program;
extern GlowProgram flame_program;
extern ShadowProgram shadow_program;
extern LineProgram line_program;
extern TextProgram text_program;
extern FireProgram fire_program;
#endif

extern int width, height;

class GlowProgram : public RectangleProgram {
    GLint mvp_location, alpha_location, vpos_location, uv_location, z_location, time_location, col_location;
    Vector3f col;
public:
    //GlowProgram() { } // XX Should go eventually
    GlowProgram(const char *name, const char *vertex_shader, const char *fragment_shader_src, const char *geometry_shader = 0) : RectangleProgram(name, vertex_shader, fragment_shader_src, geometry_shader) { }
    void init() override {
        cout << "Glow init" << endl;
        ProgramBase::init();
        cout << "Glow init done" << endl;
        mvp_location = uniform("MVP");
        alpha_location = uniform("alpha");
        z_location = uniform("z");
        time_location = uniform("time");
        col_location = uniform("col");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        unbindVertexArray();
    }
    void setColor(const Vector3f &colour) {
        col = colour;
    }
    void set(const Mat44 &mvp, float alpha, float z, float time) override {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
        glUniform1f(alpha_location, alpha);
        glUniform1f(z_location, z);
        glUniform1f(time_location, time);
        glUniform3f(col_location, col(0), col(1), col(2));
    }
};

class ShadowProgram : public ProgramBase {
    GLint mvp_location, vpos_location, alpha_location, z_location;
public:
    //ShadowProgram () { } // XX Should go eventually
    ShadowProgram(const char *name, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader = 0) : ProgramBase(name, vertex_shader, fragment_shader, geometry_shader) { }
    void init() override {
        ProgramBase::init();
        mvp_location = uniform("MVP");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        alpha_location = uniform("alpha");

        unbindVertexArray();
    }
    void set(const Mat44 &mvp, float alpha, float z) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(alpha_location, alpha);
        glUniform1f(z_location, z);
    }
    void draw() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

class LineProgram : public ProgramBase {
    GLint mvp_location, brightness_location, vpos_location, vcol_location, ratio_location;
public:
    //LineProgram () { } // XX Should go eventually
    LineProgram(const char *name, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader = 0) : ProgramBase(name, vertex_shader, fragment_shader, geometry_shader) { }
    void init() override {
        ProgramBase::init();
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

class TextProgram : public ProgramBase {
    GLint mvp_location, brightness_location, vpos_location, color_location, uv_location;
public:
    //TextProgram () { } // XX Should go eventually
    TextProgram(const char *name, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader = 0) : ProgramBase(name, vertex_shader, fragment_shader, geometry_shader) { }
    void init() override {
        ProgramBase::init();
        mvp_location = uniform("MVP");
        brightness_location = uniform("brightness");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        color_location = uniform("color");

        unbindVertexArray();
    }
    void set(const Mat44 &mvp, float brightness, RGB rgb) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(brightness_location, brightness);
        glUniform3f(color_location, rgb.r, rgb.g, rgb.b);
    }
    void draw() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

class FireProgram : public ProgramBase {
    GLint mvp_location, z_location, vpos_location, uv_location, time_location;
public:
    //FireProgram () { } // XX Should go eventually
    FireProgram(const char *name, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader = 0) : ProgramBase("fire", vertex_shader, fragment_shader, geometry_shader) { }
    void init() override {
        ProgramBase::init();
        mvp_location = uniform("MVP");
        z_location = uniform("z");

        glGenVertexArrays(1, &vao);
        bindVertexArray();

        vpos_location = attrib("vPos", 2, 7, 0);
        uv_location = attrib("uvCoord", 2, 7, 5);

        time_location = uniform("time");

        unbindVertexArray();
    }
    void set(const Mat44 &mvp, float time, float z) {
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(time_location, time);
        glUniform1f(z_location, z);
    }
};

//extern GLint attribute_coord, uniform_tex;
#endif
