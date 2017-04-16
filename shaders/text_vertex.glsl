#version 410

uniform mat4 MVP;

in vec2 vPos;
in vec2 uvCoord;

out vec2 UV;

void main() {
    UV = uvCoord;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
