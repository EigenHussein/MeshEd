#version 460 core

struct Element{
    vec4 position;
    vec4 color;
};

layout(std430, binding = 0) buffer up {
    Element Line[];
};

layout(std430, binding = 1) buffer f1 {
    Element Line1[];
};

layout(std430, binding = 2) buffer f2 {
    Element Line2[];

};
uniform float t;
layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    Line[idx].position=Line1[idx].position+(Line2[idx].position-Line1[idx].position)*t;
    //Line[idx].color=vec4(1,1,1,1);

//     Line[idx].position=vec4(float(idx));
//     Line1[idx].position=vec4(float(idx));
//     Line2[idx].position=vec4(float(idx));
 }