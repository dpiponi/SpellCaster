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
    for (int i = 0; i < 4; ++i) {
        r += s*noise2(f*vec3(uv, t));
        s *= 0.65;
        f *= 1.0/0.65;
    }

    return r;
}

void main() {
    float u = 0.5+2*(UV.x-0.5);
    float v = 0.5+2*(UV.y-0.5);
    float shape1 = smoothstep(-2.0, 0.25, u)*smoothstep(3.0, 0.75, u);
    float shape2 = smoothstep(-1.5, 0.0, 1.0-v)*smoothstep(3.0, 1.0, 1.0-v);
    float shape = 1-shape1*shape2;
    vec2 n = shape*(UV+1.0*fbm(2.0*vec2(2.0*u, v*0.5)+vec2(0, 0.5*time), 0.5*time));
    float a = dFdx(n.x);
    float b = dFdx(n.y);
    float c = dFdy(n.x);
    float d = dFdy(n.y);
    float a0 = dFdx(UV.x);
    float b0 = dFdx(UV.y);
    float c0 = dFdy(UV.x);
    float d0 = dFdy(UV.y);
    float r = 0.01*(abs(a0*d0-b0*c0)/abs(a*d-b*c));
    vec3 col1 = vec3(1.0, 0.1, 0.0);
    vec3 col2 = vec3(1.0, 0.5, 0.2);
    float shapex = shape1*shape2;
    frag_colour = vec4(mix(col1, col2, 0.5*r*alpha), r*pow(shapex,40)*alpha);
}
