#version 410
in vec2 coord;
out vec4 frag_colour;
void main() {
    //float x = (coord.x-0.5)*(coord.x-0.5)+(coord.y-0.5)*(coord.y)*0.5;
    float b = 0.1;
    float x = coord.x < -(1.0-b) ? (coord.x+(1.0-b)) : (coord.x > 1.0-b ? coord.x-(1.0-b) : 0);
    float y = coord.y < -(1.0-b) ? (coord.y+(1.0-b)) : (coord.y > 1.0-b ? coord.y-(1.0-b) : 0);
    float i = x*x/(b*b)+y*y/(b*b);
    frag_colour = vec4(0.0, 0.0, 0.0, 0.25*(1.0-i));
}
