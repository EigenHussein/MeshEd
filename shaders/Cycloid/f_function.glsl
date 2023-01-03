#version 460 core

#define CM_SINGLE 1
#define CM_PERVERTEX 2
#define CM_PERPOSITION 4

uniform uint ColorMode = 1;
uniform vec4 SolidColor = vec4(0, 1, 0.62, 1);

in vec3 vPosition;
in vec4 vColor;

out vec4 FragCol;

void main2(){
    FragCol = vec4(1,1,1,1);
}

void main() {

    if(ColorMode == CM_SINGLE) {
        FragCol = SolidColor;
    } 
    else if(ColorMode == CM_PERVERTEX) {
        FragCol = vColor;
    } 
    else if(ColorMode == CM_PERPOSITION) {
        FragCol = vec4(abs(vPosition), 1);
    }
}