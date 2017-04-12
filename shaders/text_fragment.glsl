#version 410
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 frag_colour;
void main() {
      frag_colour = vec4(1.0, 1.0, 1.0, texture(tex, texcoord).r) * color;
}
