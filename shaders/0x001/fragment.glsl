#version 460 core

in vec3 vPos;
in vec3 vNorm;

layout(location = 0) uniform vec4 iColor;

out vec4 FragColor_;

const vec3 lightPosition=vec3(10,-10,10);

void main() {

    //FragColor_ = vec4(0.5 + 0.5 * vPos.xyz, 1);
    float c= dot(normalize(lightPosition-vPos),normalize(vNorm));
    FragColor_ = vec4(max(c,0.4)*vPos,1);//iColor;

}