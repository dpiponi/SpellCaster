#version 410

uniform mat4 MVP;
uniform float z;

in vec2 vPos;
in vec2 uvCoord;

out vec2 UV;

void main() {
    UV = uvCoord;
    gl_Position = MVP * vec4(vPos, z, 1.0);
}
