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
        s *= 0.5;
        f *= 1.0/0.5;
    }

    return r;
}

void main() {
    vec2 n = UV+4.0*fbm(1.0*vec2(UV.x, UV.y*0.5)+vec2(0, 0.25*time), 0.15*time);
    float a = dFdx(n.x);
    float b = dFdx(n.y);
    float c = dFdy(n.x);
    float d = dFdy(n.y);
    //float r = 1.0-0.2*exp(0.00008/abs(a*d-b*c));
    float shape1 = smoothstep(0.0, 5.0, 2.5-25.0*(UV.s-0.5)*(UV.s-0.5));
    float shape2 = smoothstep(0.0, 1.0, 3.5-5.0*UV.y);
    float r = 0.00004/abs(a*d-b*c)*shape1*shape2;
    //r = smoothstep(0.0, 2.0, r);
    vec3 col1 = vec3(0.9, 0.2, 0.0);
    vec3 col2 = vec3(1.0, 0.6, 0.2);
    //r += 2.0*UV.y+0.5;
    frag_colour = vec4(mix(col1, col2, r), r);
#if 0
    vec2 n = fbm(6.0*vec2(UV.s, 0.4*pow(UV.t, 1.2))+vec2(0, 2.0*time), time);
    float u = UV.s-0.60+0.125*n.x;
    float v = 1.0-UV.t;
    float opacity = 2*smoothstep(-1.0, 1.0, -0.75+4.0*(v - 200.0*u*u*u*u))*(0.25+0.75*n.y);
    //vec3 color = mix(vec3(0.0, 0.0, 0.0), mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.8, 0.2), n.x), n.y);
    vec3 color = mix(vec3(1.0, 0.1, 0.0), vec3(1.0, 0.5, 0.2), n.x);
    frag_colour = vec4(color, opacity);
#endif
}
