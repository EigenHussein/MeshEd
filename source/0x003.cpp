#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Meshs.h>
#include <MeshGen.h>
#include <glm/glm.hpp>
#include <math.h>
#include <chrono>
using namespace std;

struct Line {
    glm::vec3 L0;
    glm::vec3 V;
    Line(const glm::vec3& origin, const glm::vec3& direction) :L0(origin), V(direction) {}
    glm::vec3 point(const float t) {
        return L0 + t * V;
    }
};

float k(glm::vec3 b) {
    return 0;
}
// class Intersection {
// private:
//     Line* L;
//     pfunction F;
//     const float dE = 0.0001;
//     float _error(const glm::vec3 p) {
//         return glm::length(L->point(p.z) - F({ p.x, p.y }));
//     };
//     glm::vec3 Grad(float (Intersection::* Gfunc)(glm::vec3), glm::vec3 p) {

//         glm::vec3 grad;
//         grad.x = ((this->*Gfunc)(p + glm::vec3(dE, 0.f, 0.f)) - (this->*Gfunc)(p)) / dE;
//         grad.y = ((this->*Gfunc)(p + glm::vec3(0.f, dE, 0.f)) - (this->*Gfunc)(p)) / dE;
//         grad.z = ((this->*Gfunc)(p + glm::vec3(0.f, 0.f, dE)) - (this->*Gfunc)(p)) / dE;
//         return grad;
//     };

// public:
//     Intersection(Line* line, pfunction func) :L(line), F(func) {

//     }
//     void start(glm::vec3 guess, float step_size, int steps) {
//         for (int i = 0;i < steps;i++) {
//             glm::vec3 gradient = Grad(_error, guess);
//             guess -= gradient * step_size;
//             cout << _error(guess) << endl;
//         }

//     }

// };
Line* L;
pfunction F;
const float dE = 0.00001;
float _error(const glm::vec3 p) {
    return glm::length(L->point(p.z) - F({ p.x, p.y }));
};
glm::vec3 Grad(float (*Gfunc)(glm::vec3), glm::vec3 p) {

    glm::vec3 grad;
    grad.x = ((Gfunc)(p + glm::vec3(dE, 0.f, 0.f)) - (Gfunc)(p)) / dE;
    grad.y = ((Gfunc)(p + glm::vec3(0.f, dE, 0.f)) - (Gfunc)(p)) / dE;
    grad.z = ((Gfunc)(p + glm::vec3(0.f, 0.f, dE)) - (Gfunc)(p)) / dE;
    return grad;
};
glm::vec4 Grad2(float (*Gfunc)(glm::vec3), glm::vec3 p) {

    glm::vec4 grad;
    grad.x = ((Gfunc)(p + glm::vec3(dE, 0.f, 0.f)) - (Gfunc)(p)) / dE;
    grad.y = ((Gfunc)(p + glm::vec3(0.f, dE, 0.f)) - (Gfunc)(p)) / dE;
    grad.z = ((Gfunc)(p + glm::vec3(0.f, 0.f, dE)) - (Gfunc)(p)) / dE;
    grad.w = (Gfunc(p + dE * glm::vec3(grad.x, grad.y, grad.z)) - (Gfunc)(p)) / dE;

    return grad;
};
void start(glm::vec3 guess, float step_size, int steps) {
    float previous_error = 1.f;
    for (int i = 0;i < steps;i++) {
        step_size = 0.02 * _error(guess) + 0.1 * previous_error;
        glm::vec3 gradient = Grad(_error, guess);
        guess -= gradient * step_size;
        cout << _error(guess) << endl;
    }
}
glm::vec3 start2(glm::vec3 guess, float step_size, int steps) {
    float previous_error = 1.f;
    for (int i = 0;i < steps;i++) {
        float _err = _error(guess);
        step_size = 0.02 * _err + 0.1 * previous_error;
        glm::vec4 gradient = Grad2(_error, guess);
        float height = _err / gradient.w;
        guess -= glm::vec3(gradient.x, gradient.y, gradient.z) * height;
        previous_error = _err;
        //cout << _error(guess) << endl;
    }
    cout << previous_error << endl;
    return guess;
}
int main() {
    L = new Line({ 1,1,5 }, { -1,-1,-1 });

    auto func = [](glm::vec2 uv) {
        return glm::vec3(uv.x, uv.y, uv.x * uv.y);
    };

    F = func;
    t_read(1);
    auto p = start2({ 0,0,0.5 }, 0.01f, 20);
    t_read(2);

    t_print(1, 2);
    cout << p.x << '\t' << p.y << '\t' << p.z << endl;

    return 0;
}