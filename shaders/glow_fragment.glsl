#version 410

uniform sampler2D sampler;
uniform float alpha;

in vec2 UV;

out vec4 frag_colour;

void main() {
    vec4 color = texture(sampler, UV);
    color = color*vec4(0.0, 0.0, 1.0, 1.0);
    frag_colour = vec4(color.x, color.y, color.z, alpha*color.w);
}
