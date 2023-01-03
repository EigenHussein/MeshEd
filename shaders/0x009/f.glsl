// #version 460 core

// out vec4 Color;

// void main() {

//     Color = vec4(0, 0.68, 1, 1);
// }



#version 460 core

const vec3 LightPosition = vec3(10, 10, 10);

out vec4 Color;

in vec3 vPos;
in vec3 vNorm;
// void main(){
//     Color=vec4(1);
// }
void main() {

    float amb = max(0.4, dot(normalize(LightPosition - vPos), normalize(vNorm.xyz)));
    vec3 tmpCol = vec3(1);//0.5 + 0.5 * vPos;
    vec3 tmpCol2 = vec3(2.f, 1.f, 1.f);
    Color = vec4(amb * (0.5 * tmpCol + 0.5 * tmpCol2), 1);
    //Color=vec4(1);
}
