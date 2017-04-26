#version 410

uniform sampler2D sampler;
uniform float alpha;
uniform float time;
uniform vec3 col;

in vec2 UV;

out vec4 frag_colour;

void main() {
    vec2 UV2 = UV+0.1*noise2(5.0*UV);
    float u = UV2.x;
    float v = UV2.y;
    u = u-0.5;
    v = v-0.5;
    float theta = atan(v, u);
    float r = sqrt(u*u+v*v);
    float scale = 1.0+0.15*sin(4*theta+3.141/2)+0.04*cos(11*theta+13.0*time)+0.03*sin(23*theta+35.0*time);
    r = r*scale;
    vec4 color;
    if (r < 0.5) {
        float s = r/0.5;
        float t = s*s*s*s*s*s*s*s;
        color = vec4(mix(vec3(0.9, 0.9, 0.9), col, 0.3+0.7*t), 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 0.0);
    }
    frag_colour = vec4(color.x, color.y, color.z, alpha*color.w);
}
