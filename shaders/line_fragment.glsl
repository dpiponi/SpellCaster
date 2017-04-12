#version 410
uniform sampler2D sampler;
uniform float brightness;

//in vec2 UV;
in vec3 fcolor;

out vec4 frag_colour;
void main() {
    frag_colour = vec4(brightness*fcolor, 1.0);
}
