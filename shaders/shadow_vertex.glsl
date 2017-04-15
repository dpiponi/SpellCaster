#version 410

uniform mat4 MVP;
uniform float z;

in vec2 vPos;

out vec2 coord;

void main() {
    coord = vPos;
    gl_Position = MVP * vec4(vPos, z, 1.0);
}
