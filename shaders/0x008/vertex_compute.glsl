#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 txcoord;

uniform mat4 Proj;
uniform mat4 View;
uniform uint model_index;

struct ModelMatrices {
    mat4 Rotation;
    mat4 Translation;
    vec4 Position;
    vec4 Direction;
    vec4 Color;

};

layout(std430, binding = 0) buffer MyBuf {
    ModelMatrices Info[];
};

out vec3 vPos;
out vec3 vNorm;
out vec2 uv;
out mat4 VSModel;
out vec4 VColor;
void main() {
    mat4 tmp_model = Info[gl_InstanceID].Translation * Info[gl_InstanceID].Rotation;
    vec4 tmp_pos = tmp_model * vec4(Info[gl_InstanceID].Color.a*vertexPosition, 1);
    gl_Position = Proj * View * tmp_pos;
    vPos = tmp_pos.xyz;
    vNorm = vertexNormal;
    uv = txcoord;
    VSModel = Info[gl_InstanceID].Rotation;
    VColor=Info[gl_InstanceID].Color;
}