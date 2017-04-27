#include <iostream>
#include <vector>

using std::string;

#include "json11/json11.hpp"
using json11::Json;

#include "shaders.h"
#include "Draw.h"
#include "read_file.h"

using std::cout;
using std::endl;
using std::vector;

int width, height;

GLuint vertex_buffer;

//GLint line_vpos_location, line_vcol_location, line_uv_location, line_mvp_location, line_brightness_location, line_ratio_location;
GLint attribute_coord, uniform_tex;
//GLint shadow_mvp_location, shadow_vpos_location;
GLuint /*vao, line_vao, shadow_vao, */text_vao;

#if 0
Program program;
GlowProgram glow_program;
GlowProgram flame_program;
ShadowProgram shadow_program;
LineProgram line_program;
TextProgram text_program;
FireProgram fire_program;
#endif

GLuint compile_shader(GLuint shader_type, const char *shader_src) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        cout << "Error compiling:\n" << shader_src << endl;
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        cout << &errorLog[0] << endl;
        exit(1);
    }

    return shader;
}

GLuint compile_program(const char *vertex_shader_src,
                       const char *fragment_shader_src,
                       const char *geometry_shader_src) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    if (geometry_shader_src) {
        GLuint geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_shader_src);
        glAttachShader(program, geometry_shader);
    }
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        cout << "Error linking:\n" << endl;
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
        cout << &errorLog[0] << endl;
        exit(1);
    }

    return program;
}

GLuint compileProgram(const char *vertex_program, const char *fragment_program, const char *geometry_program) {
    //shader.dump();
    string vertex_shader_src = read_file(vertex_program);
    string fragment_shader_src = read_file(fragment_program);
    cout << "vertex_program=" << endl << vertex_shader_src << endl;
    cout << "fragment_program=" << endl << fragment_shader_src << endl;
    if (geometry_program) {
        cout << "Compiling geometry!!!" << endl;
        string geometry_shader_src = read_file(geometry_program);
        return compile_program(vertex_shader_src.c_str(),
                               fragment_shader_src.c_str(),
                               geometry_shader_src.c_str()
                               );
    } else {
        return compile_program(vertex_shader_src.c_str(),
                               fragment_shader_src.c_str(),
                               0
                               );
    }
}

Program rectangleProgram("rect", "shaders/vertex.glsl", "shaders/fragment.glsl");
GlowProgram glowProgram("glow", "shaders/glow_vertex.glsl", "shaders/glow_fragment.glsl");
GlowProgram flameProgram("flame", "shaders/flame_vertex.glsl", "shaders/flame_fragment.glsl");
//FireProgram fireProgram("fire", "shaders/fire_vertex.glsl", "shaders/fire_fragment.glsl");
LineProgram lineProgram("line", "shaders/line_vertex.glsl", "shaders/line_fragment.glsl", "shaders/line_geometry.glsl");
TextProgram textProgram("text", "shaders/text_vertex.glsl", "shaders/text_fragment.glsl");
ShadowProgram shadowProgram("shadow", "shaders/shadow_vertex.glsl", "shaders/shadow_fragment.glsl");

void ProgramRegistry::init() {
    auto registry = getRegistry();
    for (auto p = registry->begin(); p != registry->end(); ++p) {
        cout << "init " << p->first << endl;
        p->second->init();
    }
}
