#version 410

uniform sampler2D sampler;
uniform float alpha;
uniform float time;
uniform vec3 col;

in vec2 UV;

out vec4 frag_colour;

vec2 fbm(vec2 uv, float t) {
    vec2 r = vec2(0.6, 0.6);
    float s = 0.4;
    float f = 1.0;
    for (int i = 0; i < 5; ++i) {
        r += s*noise2(f*vec3(uv, t));
        s *= 0.75;
        f *= 1.0/0.75;
    }

    return r;
}

void main() {
    vec2 n = fbm(6.0*vec2(UV.s, 0.4*pow(UV.t, 1.2))+vec2(0, 2.0*time), time);
    float u = UV.s-0.60+0.125*n.x;
    float v = 1.0-UV.t;
    float opacity = 2*smoothstep(-1.0, 1.0, -0.75+4.0*(v - 200.0*u*u*u*u))*(0.25+0.75*n.y);
    //vec3 color = mix(vec3(0.0, 0.0, 0.0), mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.8, 0.2), n.x), n.y);
    vec3 color = mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.8, 0.2), n.x);
    frag_colour = vec4(color, opacity);
}
