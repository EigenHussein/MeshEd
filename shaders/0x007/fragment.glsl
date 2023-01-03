#version 460 core

in vec3 vPos;
in vec4 vCol;

layout(location = 0) uniform vec4 iColor;

out vec4 FragColor_;

void main() {

    //FragColor_ = vec4(0.5 + 0.5 * vPos.xyz, 1);
    FragColor_ = vCol;

}