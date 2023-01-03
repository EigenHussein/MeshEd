#version 460 core

uint nElemets = 256 + 1;//wire element
uint nCE = 100 + 1; //cross section elements

const vec2 start_end = vec2(-1, 1);
float dt = (start_end[1] - start_end[0])/float(nElemets - 1);
struct MVertex {
    vec4 Position;
    //vec4 Normal;
};

layout(std430, binding = 1) buffer Cir{
    vec4 Circle[ /* nCE */];
};

layout(std430, binding = 0) buffer Wire {
    MVertex WireElements[/*  (nCE)*2*(nElemets)  */];
};

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

vec3 wire_path(float t);
mat3 GetTransformation(vec3 p0, vec3 p1) {

    vec3 w = normalize(p0 - p1);
    vec3 u = normalize(cross(vec3(0.f, 0.f, 1.f), w));
    vec3 v = normalize(cross(w, u));//the amount of insecurity in this line... holy god
    return mat3(u, v, w);

}
void Fill(uvec2 idx){

    float t = start_end[0] + dt * idx.x;
    vec3 p1= wire_path(t);
    vec3 p2= wire_path(t + dt);
    mat3 RotMat = GetTransformation(p1, p2);
    for (int j = 0; j < nCE; j++) {
        WireElements[j + idx.x * nCE].Position.xyz = p1 + RotMat * Circle[j].xyz;
    }
    
}
// void Fill(uvec2 idx){
//     float t=start_end[0]+dt*idx.x;
//     vec3 p1=wire_path(t);
//     vec3 p2=wire_path(t+dt);
//     mat3 RotMat = GetTransformation(p1, p2);
//     WireElements[idx.x*2+0].Position.xyz=p1;
//     WireElements[idx.x*2+1].Position.xyz=p1+RotMat*vec3(1,0,0);
// }
void main() {

    uvec2 id = gl_GlobalInvocationID.xy;
    Fill(id);
}

vec3 wire_path(float t) {
    return vec3(5*cos(5*t), 5*sin(5*t), t);
}