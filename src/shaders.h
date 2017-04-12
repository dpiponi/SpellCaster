#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>

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

#if 0
GLuint compile_program(const char *, const char *);
GLuint compile_line_shader();
GLuint compile_text_shader();
GLuint compile_shadow_shader();
#endif

extern GLuint program;
extern GLuint line_program;
extern GLuint text_program;
extern GLuint shadow_program;

void connect_shader();
void connect_line_shader();
void connect_text_shader();
void connect_shadow_shader();

extern int width, height;

extern GLint vpos_location, vcol_location, uv_location, mvp_location, brightness_location;
extern GLint is_alpha_location;
extern GLint line_vpos_location, line_vcol_location, line_uv_location, line_mvp_location, line_brightness_location, line_ratio_location;
extern GLint attribute_coord, uniform_tex;
extern GLint shadow_mvp_location, shadow_vpos_location;
#endif
