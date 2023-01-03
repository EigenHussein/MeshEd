#version 460 core

const vec3 LightPosition = vec3(10, 10, 10);

out vec4 Color;

in vec3 vPos;
in vec3 vNorm;
in vec2 uv;

in mat4 VSModel;
in vec4 VColor;
uniform sampler2D NormalMapTexture;
uniform vec3 CameraPosition=vec3(5,5,5);
void main() {
    vec3 normal_val = 2 * texture(NormalMapTexture, uv).rgb - 1;
    vec4 newNorm = normalize(VSModel * vec4(normal_val, 0));
    vec3 ref=normalize(reflect(vPos-LightPosition,newNorm.xyz));
    vec3 look=normalize(CameraPosition-vPos);
    float diffuse=pow(dot(ref,look),16);

    float amb = max(0,diffuse)+max(0.14, dot(normalize(LightPosition - vPos), normalize(newNorm.xyz)));
    vec3 tmpCol = 0.5 + 0.5 * vPos;
    vec3 tmpCol2 = vec3(1.f, 0.f, 0.f);
    Color = vec4(amb *VColor.xyz,1);//Color = vec4(amb * (0.5 * tmpCol + 0.5 * tmpCol2), 1);
}
