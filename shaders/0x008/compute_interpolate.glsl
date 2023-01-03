#version 460 core

#define pi 3.1415926535

struct VectorField {//Vector Fileld
    vec4 Position;
    vec4 Direction;
};

struct ModelMatrices {//Interpolation
    mat4 Rotation;
    mat4 Translation;
    vec4 Position;
    vec4 Direction;
    vec4 Color;

};

#define VFS 64
#define BFS 16

layout(std430, binding = 1) buffer VF_ {
    VectorField VF[VFS][VFS][VFS];
};
layout(std430, binding = 0) buffer BF_ {
    ModelMatrices Info[BFS][BFS][BFS];
};


layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

mat4 Rotate(vec3 ref) {
mat4 tmp;
    tmp[3] = vec4(0, 0, 0, 1);
    tmp[2] = vec4(ref, 0);
    tmp[1] = vec4(normalize(cross(vec3(0.7071, 0, 0.7071), ref)), 0);
    tmp[0] = vec4(normalize(cross(cross(vec3(0.7071, 0, 0.7071), ref), ref)), 0);
    return tmp;

}
mat4 Translate(vec3 ref) {
    mat4 tmp;
    tmp[3] = vec4(ref, 1);
    tmp[2] = vec4(0, 0, 1, 0);
    tmp[1] = vec4(0, 1, 0, 0);
    tmp[0] = vec4(1, 0, 0, 0);
    return tmp;
}
vec3 Field(vec3 p) {
    return normalize(vec3(- p.y, p.x, p.z));
}

    uniform vec3 steps = vec3(0.375);
    uniform vec3 head_vf=vec3(-12,-12,-12);
    uniform vec3 head = vec3(- 8, - 8, - 8);

#define LERP(A,B,t) (A+(B-A)*t)

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

vec3 interpolate2(vec3 p) {

    vec3 relative_position = p - head_vf;
    vec3 steps_position = relative_position / steps;
    vec3 reference_position = floor(steps_position);

    ivec3 idx = ivec3(reference_position);
    vec3 dm = steps_position - reference_position;

    vec3 z=
    (1-dm.z)*(1-dm.y)*(1-dm.x)*VF[idx.x][idx.y][idx.z].Direction.xyz+
    (1-dm.z)*(1-dm.y)*(dm.x)*VF[idx.x+1][idx.y][idx.z].Direction.xyz+

    (1-dm.z)*(dm.y)*(1-dm.x)*VF[idx.x][idx.y+1][idx.z].Direction.xyz+
    (1-dm.z)*(dm.y)*(dm.x)*VF[idx.x+1][idx.y+1][idx.z].Direction.xyz+

    (dm.z)*(1-dm.y)*(1-dm.x)*VF[idx.x][idx.y][idx.z+1].Direction.xyz+
    (dm.z)*(1-dm.y)*(dm.x)*VF[idx.x+1][idx.y][idx.z+1].Direction.xyz+

    (dm.z)*(dm.y)*(1-dm.x)*VF[idx.x][idx.y+1][idx.z+1].Direction.xyz+
    (dm.z)*(dm.y)*(dm.x)*VF[idx.x+1][idx.y+1][idx.z+1].Direction.xyz;

    return z;

}

float sigmoid(float t){
    return 1.f/(1.f+exp(-t));
}
uniform float a;
uniform float b;
void main() {

    uvec3 id = gl_GlobalInvocationID;
    vec3 location = Info[id.x][id.y][id.z].Position.xyz;
    vec3 rotation = Info[id.x][id.y][id.z].Direction.xyz;
    vec4 field = vec4(interpolate2(location), 1);//vec4(interpolate(location), 1);
    Info[id.x][id.y][id.z].Position.xyz = location +  0.1*field.xyz;
    //Info[id.x][id.y][id.z].RotVec.xyz = field.xyz;
    Info[id.x][id.y][id.z].Rotation = Rotate(normalize(field.xyz));
    Info[id.x][id.y][id.z].Translation = Translate(location);
    Info[id.x][id.y][id.z].Color.a=sigmoid(length(field.xyz));
    Info[id.x][id.y][id.z].Color.xyz=LERP(vec3(0,0,1),vec3(1,0,0),sigmoid(a*length(field.xyz)+b));
}