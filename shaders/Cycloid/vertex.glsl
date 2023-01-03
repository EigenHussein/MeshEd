#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 col;
layout(location = 3) in vec3 uv;


layout(location = 0) uniform vec3 offset;//unique uniform location (to avoid getUniformLocation)
uniform mat4 Proj;
uniform mat4 View;

out vec3 vPosition;
out vec4 vColor;

void main() {

    gl_Position = Proj * View * vec4(pos+offset, 3);
    vPosition = pos;
    vColor = col;

}