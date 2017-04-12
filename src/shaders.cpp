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

GLint vpos_location, vcol_location, uv_location, mvp_location, brightness_location;
GLint is_alpha_location;
GLint line_vpos_location, line_vcol_location, line_uv_location, line_mvp_location, line_brightness_location, line_ratio_location;
GLint attribute_coord, uniform_tex;
GLint shadow_mvp_location, shadow_vpos_location;

#if 0
const char* vertex_shader_src = R"END(
#version 410

uniform mat4 MVP;

in vec3 vCol;
in vec2 vPos;
in vec2 uvCoord;

out vec2 UV;
out vec3 col;
void main() {
    UV = uvCoord;
    col = vCol;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
)END";

const char* fragment_shader_src = R"END(
#version 410

uniform sampler2D sampler;
uniform float brightness;
uniform float is_alpha;

in vec3 col;
in vec2 UV;

out vec4 frag_colour;

void main() {
    if (is_alpha==0.0) {
        float level = textureQueryLod(sampler, UV).x;
        level = max(0.0, level-0.5);
        vec4 color = textureLod(sampler, UV, level);
        frag_colour = brightness*color;
    } else {
        vec3 color = col;
        frag_colour = vec4(brightness*color, texture(sampler, UV).r);
    }
}
)END";
#endif

#if 0
// Line shader
//
const char* vertex_shader_line = R"END(
#version 410
uniform mat4 MVP;
in vec3 vCol;
in vec2 vPos;
in vec2 uvCoord;
out vec2 UV;
out vec3 color;
void main() {
    UV = uvCoord;
    color = vCol;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
)END";
#endif

GLuint program;
GLuint line_program;
GLuint text_program;
GLuint shadow_program;

#if 0
const char* fragment_shader_line = R"END(
#version 410
uniform sampler2D sampler;
uniform float brightness;
in vec2 UV;
in vec3 color;
out vec4 frag_colour;
void main() {
    frag_colour = vec4(brightness*color, 1.0);
}
)END";
#endif

#if 0
const char* vertex_shader_text = R"END(
#version 410
in vec4 coord;
out vec2 texcoord;
void main(void) {
      gl_Position = vec4(coord.xy, 0, 1);
      texcoord = coord.zw;
}
)END";

const char *fragment_shader_text = R"END(
#version 410
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 frag_colour;
void main() {
      frag_colour = vec4(1.0, 1.0, 1.0, texture(tex, texcoord).r) * color;
}
)END";
#endif

#if 0
const char* vertex_shader_shadow = R"END(
#version 410
uniform mat4 MVP;
in vec2 vPos;
out vec2 coord;
void main() {
    coord = vPos;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
)END";

const char *fragment_shader_shadow = R"END(
#version 410
in vec2 coord;
out vec4 frag_colour;
void main() {
    //float x = (coord.x-0.5)*(coord.x-0.5)+(coord.y-0.5)*(coord.y)*0.5;
    float b = 0.1;
    float x = coord.x < -(1.0-b) ? (coord.x+(1.0-b)) : (coord.x > 1.0-b ? coord.x-(1.0-b) : 0);
    float y = coord.y < -(1.0-b) ? (coord.y+(1.0-b)) : (coord.y > 1.0-b ? coord.y-(1.0-b) : 0);
    float i = x*x/(b*b)+y*y/(b*b);
    frag_colour = vec4(0.0, 0.0, 0.0, 0.25*(1.0-i));
}
)END";
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

GLuint compile_program(const char *vertex_shader_src, const char *fragment_shader_src, const char *geometry_shader_src) {
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

GLuint compileProgram(Json shader) {
    shader.dump();
    string vertex_shader_src = read_file(shader["vertex"].string_value().c_str());
    string fragment_shader_src = read_file(shader["fragment"].string_value().c_str());
    if (shader["geometry"] != Json()) {
        cout << "Compiling geometry!!!" << endl;
        string geometry_shader_src = read_file(shader["geometry"].string_value().c_str());
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

void connect_shader() {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    mvp_location = glGetUniformLocation(program, "MVP");
    brightness_location = glGetUniformLocation(program, "brightness");
    vpos_location = glGetAttribLocation(program, "vPos");
    is_alpha_location = glGetUniformLocation(program, "is_alpha");
    vcol_location = glGetAttribLocation(program, "vCol");
    uv_location = glGetAttribLocation(program, "uvCoord");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) (sizeof(float) * 2));

    glEnableVertexAttribArray(uv_location);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) (sizeof(float) * 5));
}

void connect_line_shader() {
    glBindBuffer(GL_ARRAY_BUFFER, line_vertex_buffer);
    line_mvp_location = glGetUniformLocation(line_program, "MVP");
    line_brightness_location = glGetUniformLocation(line_program, "brightness");
    line_vpos_location = glGetAttribLocation(line_program, "vPos");
    line_vcol_location = glGetAttribLocation(line_program, "vCol");
    line_ratio_location = glGetUniformLocation(line_program, "ratio");
//    line_uv_location = glGetAttribLocation(line_program, "uvCoord");

    glEnableVertexAttribArray(line_vpos_location);
    glVertexAttribPointer(line_vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) 0);

    glEnableVertexAttribArray(line_vcol_location);
    glVertexAttribPointer(line_vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) (sizeof(float) * 2));

//    glEnableVertexAttribArray(line_uv_location);
//    glVertexAttribPointer(line_uv_location, 2, GL_FLOAT, GL_FALSE,
//                          sizeof(float) * 7, (void*) (sizeof(float) * 5));

//    glEnableVertexAttribArray(line_uv_location);
//    glVertexAttribPointer(line_uv_location, 2, GL_FLOAT, GL_FALSE,
//                          sizeof(float) * 7, (void*) (sizeof(float) * 5));
}

void connect_text_shader() {
    attribute_coord = glGetAttribLocation(text_program, "coord");
    uniform_tex = glGetUniformLocation(text_program, "tex");
}

void connect_shadow_shader() {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    shadow_mvp_location = glGetUniformLocation(shadow_program, "MVP");
    shadow_vpos_location = glGetAttribLocation(shadow_program, "vPos");

    glEnableVertexAttribArray(shadow_vpos_location);
    glVertexAttribPointer(shadow_vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 7, (void*) 0);
}

#if 0
GLuint compile_shadow_shader() {
    GLuint vertex_shader, fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_shadow, NULL);
    glCompileShader(vertex_shader);
    GLint isCompiled = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        cout << "Error compiling vertex shader " << vertex_shader << endl;
        GLint maxLength = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);
        cout << &errorLog[0] << endl;
        exit(1);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_shadow, NULL);
    glCompileShader(fragment_shader);
    isCompiled = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        cout << "Error frag" << fragment_shader_shadow << endl;
        GLint maxLength = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);
        cout << &errorLog[0] << endl;
        exit(1);
    }

    int shadow_program = glCreateProgram();
    glAttachShader(shadow_program, vertex_shader);
    glAttachShader(shadow_program, fragment_shader);
    glLinkProgram(shadow_program);
    return shadow_program;
}
#endif
