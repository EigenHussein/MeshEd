#version 460 core

uniform vec4 uColor = vec4(1, 1, 1, 1);
out vec4 Color;
in vec3 vPosition;
void main() {
    float dist=length(vPosition);
    float color_decay=min(1,1/(0.1+0.5*dist));
    Color =color_decay* uColor;

}