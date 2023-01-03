#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

uniform mat4 Proj;
uniform mat4 View;

out vec3 vPos;
out vec4 vCol;
void main() {

    gl_Position = Proj * View * vec4(pos, 1);

    vPos = pos;
    vCol = col;

}