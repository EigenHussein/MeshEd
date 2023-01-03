#version 460 core
#define pi 3.1415926535

struct VectorField {
    vec4 Position;
    vec4 Direction;
};

struct ModelMatrices {
    mat4 Rotation;
    mat4 Translation;
    vec4 Position;
    vec4 Direction;
    vec4 Color;

};


const uint FieldSize = 64;
const uint InterSize = 16;
#define DIFF ((FieldSize-InterSize)/2)


layout(std430, binding = 0) buffer BF_ {
    //ModelMatrices Info[InterSize][InterSize][InterSize];
    ModelMatrices Info[];//[InterSize*InterSize*InterSize];
};

layout(std430, binding = 1) buffer VF_ {
    //VectorField VF[FieldSize][FieldSize][FieldSize];
    VectorField VF[];//[FieldSize*FieldSize*FieldSize];
};



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

vec3 path(float t);
vec3 Magnet(vec3 p) {
    vec3 acc = vec3(0);
    const float dt = 0.001;
    for(float i = -1;
    i < 1.f;
    i += 0.001) {
        vec3 point_ = path(i);
        float R = length(p - point_);
        vec3 L = normalize((path(i + dt) - point_) / dt);
        vec3 D = normalize(p - point_);
        acc = acc + cross(L, D) / pow(R, 2) * dt*4;//*****************4*/
    }
return acc;
}
vec3 Field(vec3 p) {
    return normalize(vec3(- p.y, p.x, 0));
}

#define LERP(A,B,t) (A+(B-A)*t)

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

const vec3 steps_vf = vec3(0.375);
const vec3 head_vf=vec3(-12,-12,-12);
const vec3 steps = vec3(1);
const vec3 head = vec3(- 8, - 8, - 8);
    
layout(local_size_x = 2, local_size_y = 2, local_size_z = 2) in;

void main() {

    uvec3 id = gl_GlobalInvocationID;
    vec3 location = head_vf + steps_vf * id;
    vec3 Value = Magnet(location);//Magnet(location);
    //float VectorLength = length(Value);
        //vec3 RotVector = normalize();
    uint index = id.x * FieldSize * FieldSize + id.y * FieldSize + id.z;
    VF[index].Position = vec4(location,0);//vec4(id.x,id.y,id.z, 0);
    VF[index].Direction = vec4(Value, 0);//vec4(Value/VectorLength,VectorLength);

    // if(
    //     /* (id.x >= 4 && id.x<20)&&
    //     (id.y >= 4 && id.y<20)&&
    //     (id.z >= 4 && id.z<20) */
    //     (id.x < 16)&&
    //     (id.y < 16)&&
    //     (id.z < 16)
    // ) {
    //     vec3 L = head + steps * id;
    //     vec3 V = Magnet(L);
    //     Info[index].Position = vec4(id.x, id.y, id.z);//vec4(L, 0);
    //     Info[index].Direction = vec4(V, 0);
    // }

}

vec3 path(float t) {
    t = 2*pi*t;
    return vec3(t*t*t,t,t);//return 5*vec3(cos(6*1.1*t)+0.0001, sin(6*1.1*t)+0.0001, t/5);//-2+2*t/pi);
}