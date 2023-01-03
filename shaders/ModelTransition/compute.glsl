#version 460 core

struct VertexInfo{
    vec4 pos;
    vec4 normal;
    vec4 color;
    //vec2 uv;
};

struct Triangle{
    VertexInfo vertices[3];
};

layout(std430, binding = 0)buffer VerInfo{
    Triangle triangles[];
};


layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;


#define DX 0.01

vec4 lerp(vec4 p1,vec4 p2,float t){
    return p1+t*(p2-p1);
}
#define RADIUS 2.f
vec4 force(vec4 p){
    vec2 direction=normalize(p.xy);
    vec4 rotation=(p.z+0.1)*vec4(-p.y,p.x,0,0);
    vec4 radial_move= vec4((p.z+p.y+p.z)*direction-p.xy,0,0);
    return rotation+radial_move;
}

vec4 triangle_center(const Triangle T){
    return (T.vertices[0].pos + T.vertices[1].pos + T.vertices[2].pos)/3.f;
}
void triangle_rescale(inout Triangle T, vec4 center,float s){
    //Triangle tmp = T;
    // vec4 c = triangle_center(T);
    T.vertices[0].pos=lerp(center,T.vertices[0].pos,s);
    T.vertices[1].pos=lerp(center,T.vertices[1].pos,s);
    T.vertices[2].pos=lerp(center,T.vertices[2].pos,s);
    // return T;

}

void triangle_move(inout Triangle T,vec4 direction){
    T.vertices[0].pos+=direction*DX;
    T.vertices[1].pos+=direction*DX;
    T.vertices[2].pos+=direction*DX;

}

// void scale_triangle(Triangle & T){

// }

void main(){

    uint id=gl_GlobalInvocationID.x;
    Triangle current_triangle=triangles[id];
    vec4 center=triangle_center(current_triangle);
    triangle_rescale(current_triangle,center,0.999);

    vec4 direction=force(center);
    triangle_move(current_triangle,direction);
    triangles[id]=current_triangle;
    //vertices[id].pos+=0.002*force(vertices[id].pos);
}