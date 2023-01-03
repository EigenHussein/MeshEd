#version 460 core

in vec4 vColor;
out vec4 Color;

void main() {

    //Color = vec4(0.34, 0.55, 1, 1);
    Color = vColor;
}