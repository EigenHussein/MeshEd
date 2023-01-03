#version 460 core

uint nElemets = 513 + 1;//wire element
uint nCE = 129 + 1; //cross section elements

const vec2 start_end = vec2(-1, 1);
float dt=(start_end[1]-start_end[0])/float(nElemets-1);

struct MVertex {
    vec4 Position;
    vec4 Normal;
};

layout(std430, binding = 3) buffer Wire {
    MVertex WireElements[/*  (nCE)*2*(nElemets)  */];
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main(){
    
    uint idx = gl_GlobalInvocationID.x;

    for(int i=0;i<nCE-1;i++){

        uint idx0 = idx*nCE+i;

        vec3 v0 = WireElements[idx0].Position.xyz;
        vec3 v1 = WireElements[idx0+1].Position.xyz;
        vec3 v2 = WireElements[idx0+nCE].Position.xyz;
        vec3 norm = -normalize(cross(v1-v0,v2-v0));
        WireElements[idx0].Normal.xyz=norm;
    
    }
    uint idx00 = idx*nCE + nCE-1;

    WireElements[idx00].Normal.xyz = WireElements[idx*nCE].Normal.xyz;

}
