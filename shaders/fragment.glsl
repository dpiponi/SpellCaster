#version 410

uniform sampler2D sampler;
uniform float brightness;

in vec3 col;
in vec2 UV;

out vec4 frag_colour;

void main() {
    vec4 color = texture(sampler, UV);
    frag_colour = brightness*color;
}
