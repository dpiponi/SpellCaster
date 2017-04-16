#version 410

uniform mat4 MVP;
uniform float z;

in vec3 vCol;
in vec2 vPos;
in vec2 uvCoord;

out vec2 UV;
out vec3 col;

void main() {
    UV = uvCoord;
    col = vCol;
    gl_Position = MVP * vec4(vPos, z, 1.0);
}
