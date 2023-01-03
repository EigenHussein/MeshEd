#version 460 core


layout (location = 0)in vec4 vPosition;
layout (location = 1)in vec3 vNormal;
layout (location = 2)in vec3 vColor;
layout (location = 3)in vec2 vUVCoords;

out vec3 vPos;
out vec3 vNorm;
out vec2 vUV;


//Uniforms
uniform mat4 Proj;
uniform mat4 View;

void main(){

    gl_Position=Proj*View*vec4(vPosition.xyz,1);

    vPos=vPosition.xyz;
    vNorm=vNormal.xyz;
    vUV=vUVCoords;
}