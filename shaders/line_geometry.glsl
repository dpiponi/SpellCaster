#version 410

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float ratio;

in vec3 gcolor[];

out vec3 fcolor;

void main()
{
    vec4 dir = gl_in[1].gl_Position-gl_in[0].gl_Position;
    dir = 0.002*normalize(dir);
//    dir.xyzw = dir.yxzw;
//    dir.y = -ratio*dir.y;
    float dx = dir.x;
    float dy = dir.y;

    gl_Position = gl_in[0].gl_Position + vec4(-dy-dx, ratio*(dx-dy), 0.0, 0.0);
    fcolor = gcolor[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(dy-dx, ratio*(-dx-dy), 0.0, 0.0);
    fcolor = gcolor[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(-dy+dx, ratio*(dx+dy), 0.0, 0.0);
    fcolor = gcolor[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(dy+dx, ratio*(-dx+dy), 0.0, 0.0);
    fcolor = gcolor[0];
    EmitVertex();

    EndPrimitive();
}
