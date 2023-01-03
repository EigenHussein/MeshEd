#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

uniform mat4 Proj;
uniform mat4 View;

out vec3 vPos;
out vec3 vNorm;

void main() {

    gl_Position = Proj * View * vec4(pos, 1);

    vPos = 0.5+0.5*pos;
    vNorm=norm;
    
}