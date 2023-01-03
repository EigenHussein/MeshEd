#ifndef NUMERICAL_HH
#define NUMERICAL_HH

#include <glm/glm.hpp>
#include <iostream>

typedef glm::vec3(*pfunction)(glm::vec2);

struct Line {
    glm::vec3 L0;
    glm::vec3 V;
    Line(const glm::vec3& origin, const glm::vec3& direction) :L0(origin), V(direction) {}
    glm::vec3 point(const float t) const {
        return L0 + t * V;
    }
};

const float dE = 0.00001;

glm::vec3 Grad(float (*Gfunc)(glm::vec3), glm::vec3 p) {

    glm::vec3 grad;
    grad.x = ((Gfunc)(p + glm::vec3(dE, 0.f, 0.f)) - (Gfunc)(p)) / dE;
    grad.y = ((Gfunc)(p + glm::vec3(0.f, dE, 0.f)) - (Gfunc)(p)) / dE;
    grad.z = ((Gfunc)(p + glm::vec3(0.f, 0.f, dE)) - (Gfunc)(p)) / dE;
    return grad;
};
// glm::vec4 Grad2(float (*Gfunc)(glm::vec3), glm::vec3 p) {

//     glm::vec4 grad;
//     grad.x = ((Gfunc)(p + glm::vec3(dE, 0.f, 0.f)) - (Gfunc)(p)) / dE;
//     grad.y = ((Gfunc)(p + glm::vec3(0.f, dE, 0.f)) - (Gfunc)(p)) / dE;
//     grad.z = ((Gfunc)(p + glm::vec3(0.f, 0.f, dE)) - (Gfunc)(p)) / dE;
//     grad.w = (Gfunc(p + dE * glm::vec3(grad.x, grad.y, grad.z)) - (Gfunc)(p)) / dE;

//     return grad;
// };
// void start(glm::vec3 guess, float step_size, int steps) {
//     float previous_error = 1.f;
//     for (int i = 0;i < steps;i++) {
//         step_size = 0.02 * _error(guess) + 0.1 * previous_error;
//         glm::vec3 gradient = Grad(_error, guess);
//         guess -= gradient * step_size;
//         std::cout << _error(guess) << std::endl;
//     }
// }
// glm::vec3 start2(glm::vec3 guess, float step_size, int steps) {
//     float previous_error = 1.f;
//     for (int i = 0;i < steps;i++) {
//         float _err = _error(guess);
//         step_size = 0.02 * _err + 0.1 * previous_error;
//         glm::vec4 gradient = Grad2(_error, guess);
//         float height = _err / gradient.w;
//         guess -= glm::vec3(gradient.x, gradient.y, gradient.z) * height;
//         previous_error = _err;
//         //cout << _error(guess) << endl;
//     }
//     std::cout << previous_error << std::endl;
//     return guess;
// }
//Line Surface Intersection

float _error(const glm::vec3 p, const Line* L, pfunction F) {
    return glm::length(L->point(p.z) - F({ p.x, p.y }));
};

class LS_Intersection {
private:

    Line* L;
    pfunction F;
    glm::vec4 Grad2(float (*Gfunc)(glm::vec3, const Line*, pfunction), glm::vec3 p) {
        glm::vec4 grad;
        grad.x = ((Gfunc)(p + glm::vec3(dE, 0.f, 0.f), L, F) - (Gfunc)(p, L, F)) / dE;
        grad.y = ((Gfunc)(p + glm::vec3(0.f, dE, 0.f), L, F) - (Gfunc)(p, L, F)) / dE;
        grad.z = ((Gfunc)(p + glm::vec3(0.f, 0.f, dE), L, F) - (Gfunc)(p, L, F)) / dE;
        grad.w = (Gfunc(p + dE * glm::vec3(grad.x, grad.y, grad.z), L, F) - (Gfunc)(p, L, F)) / dE;

        return grad;
    };
public:

    LS_Intersection(Line* line, pfunction surface) :L(line), F(surface) {

    }

    glm::vec3 Solve(glm::vec3 guess, float step_size, int iterations) {
        float previous_error = 1.f;
        for (int i = 0;i < iterations;i++) {
            float _err = _error(guess, L, F);
            step_size = 0.02 * _err + 0.1 * previous_error;
            glm::vec4 gradient = Grad2(_error, guess);
            float height = _err / gradient.w;
            guess -= glm::vec3(gradient.x, gradient.y, gradient.z) * height;
            previous_error = _err;
            //cout << _error(guess) << endl;
        }
        return guess;
    }

};
#endif