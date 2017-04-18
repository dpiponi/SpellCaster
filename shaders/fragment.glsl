#version 410

uniform sampler2D sampler;
uniform float brightness;
uniform float is_alpha;

in vec3 col;
in vec2 UV;

out vec4 frag_colour;

void main() {
    if (is_alpha==0.0) {
        //float level = textureQueryLod(sampler, UV).x;
        //level = max(0.0, level-0.5);
        //vec4 color = textureLod(sampler, UV, level);
        vec4 color = texture(sampler, UV);
        frag_colour = brightness*color;
    } else {
        vec3 color = col;
        frag_colour = vec4(brightness*color, texture(sampler, UV).r);
    }
}
