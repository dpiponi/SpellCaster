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
