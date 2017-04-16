#version 410

uniform sampler2D sampler;
uniform float brightness;
uniform vec3 color;

in vec2 UV;

out vec4 frag_colour;

void main() {
    //frag_colour = vec4(color*brightness, texture(sampler, UV).r);
    float f = texture(sampler, UV).r;
    frag_colour = vec4(f, f, 0.9*f, f);
}
