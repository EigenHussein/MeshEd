#version 460 core


in vec3 vPos;
in vec3 vNorm;
in vec2 vUV;

const vec3 lightPosition = vec3(3,3,3);
out vec4 Color;

void main(){

    Color = vec4(0.5+0.5*vPos,1);
    
}