#version 410

uniform sampler2D sampler;
uniform float time;

in vec2 UV;

out vec4 frag_colour;

void main() {
    // Generate noisy x value
    vec2 n0Uv = vec2(UV.x*1.4 + 0.01, UV.y + time*0.69);
    vec2 n1Uv = vec2(UV.x*0.5 - 0.033, UV.y*2.0 + time*0.12);
    vec2 n2Uv = vec2(UV.x*0.94 + 0.02, UV.y*3.0 + time*0.61);
    float n0 = (texture(sampler, n0Uv).w-0.5)*2.0;
    float n1 = (texture(sampler, n1Uv).w-0.5)*2.0;
    float n2 = (texture(sampler, n2Uv).w-0.5)*2.0;
    float noiseA = clamp(n0 + n1 + n2, -1.0, 1.0);

    // Generate noisy y value
    vec2 n0UvB = vec2(UV.x*0.7 - 0.01, UV.y + time*0.27);
    vec2 n1UvB = vec2(UV.x*0.45 + 0.033, UV.y*1.9 + time*0.61);
    vec2 n2UvB = vec2(UV.x*0.8 - 0.02, UV.y*2.5 + time*0.51);
    float n0B = (texture(sampler, n0UvB).w-0.5)*2.0;
    float n1B = (texture(sampler, n1UvB).w-0.5)*2.0;
    float n2B = (texture(sampler, n2UvB).w-0.5)*2.0;
    float noiseB = clamp(n0B + n1B + n2B, -1.0, 1.0);

    vec2 finalNoise = vec2(noiseA, noiseB);
    float perturb = (1.0 - UV.y) * 0.35 + 0.02;
    finalNoise = (finalNoise * perturb) + UV - 0.02;

    vec4 color = texture(sampler, finalNoise);
    color = vec4(color.x*2.0, color.y*0.9, (color.y/color.x)*0.2, 1.0);
    finalNoise = clamp(finalNoise, 0.05, 1.0);
    color.w = texture(sampler, finalNoise).z*2.0;
    color.w = color.w*texture(sampler, UV).z;
    frag_colour = color;

    //frag_colour = texture(sampler, vec2(noiseA, noiseB));
    //frag_colour = texture(sampler, n2UvB);
    //frag_colour = vec4(n0, n0, n0, 1.0);
}
