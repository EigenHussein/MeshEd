#version 460 core

layout(location = 0) in vec3 vertexPosition;
//layout(location = 1) in vec3 vertexNormal;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

out vec3 vPos;
out vec3 vNorm;

void main() {

    vec4 tmp_pos = vec4(vertexPosition, 1);
    gl_Position = Proj * View * tmp_pos;
    vPos = tmp_pos.xyz;
    //vNorm = vertexNormal;
    
}