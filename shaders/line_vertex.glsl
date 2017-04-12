#version 410
uniform mat4 MVP;

in vec2 vPos;
//in vec2 uvCoord;
in vec3 vCol;

//out vec2 UV;
out vec3 gcolor;

void main() {
//    UV = uvCoord;
    gcolor = vCol;
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
}
