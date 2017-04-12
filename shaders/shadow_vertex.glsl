#version 410
uniform mat4 MVP;
in vec2 vPos;
out vec2 coord;
void main() {
    coord = vPos;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
