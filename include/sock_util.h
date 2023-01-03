
#ifndef SOCK_UTILS_HEHE_HH
#define SOCK_UTILS_HEHE_HH
#include <../../../Dev/CPlusPlus/dev_sock/include/socket_util.h>
struct vec3 {
    float v[3];
    vec3() :v({ 0,0,0 }) {}
    vec3(const float* vals) :v({ vals[0],vals[1],vals[2] }) {}
    vec3(const float v1, const float v2, const float v3) :v({ v1,v2,v3 }) {}
};

struct time_packet {
    vec3 val;
    float t;
    time_packet(const vec3& _p, const float _t) :val(_p), t(_t) {}
    time_packet() :val(), t(0) {}
};

#endif
