#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>

#define _USE_TIMERS
#include <complex>
#include <gl_init.h>
// Latex
#include <EqTexGL.h>
#include <SVGParser.h>
#include <memory.h>
#include <memory>
using namespace std;

const int width = 1000, height = 1000;
#define IDENTITY glm::identity<glm::mat4>()

#include <ShapeUtil.h>

ostream &operator<<(ostream &o, const glm::vec4 &p) {
    o << "{" << p.x << ',' << p.y << ',' << p.z << ',' << p.a << "}";
    return o;
}

int main2() {

    glm::vec4 arr[100] = {};
    unsigned int indices[100] = {};
    unsigned int *ind_ptr = indices;
    for (int i = 0; i < 4; i++) {
        arr[i] = {i, i, i, i};
    }
    glm::vec4 *tmp = &arr[4];
    glm::vec4 **tmp2 = &tmp;
    cout << ind_ptr << endl;
    ShapeGenerators::Quad({{9, 9, 9}, {8, 8, 8}, {7, 7, 7}, {6, 6, 6}}, tmp2, 0, &ind_ptr);
    cout << ind_ptr << endl;
    for (int i = 0; i < 10; i++) {
        cout << indices[i] << '\t' << arr[i] << endl;
    }
    return 0;
}

int main() {

    GL::Core Core = GL_CORE_DEFAULT(1000, 1000);
    Cycloid::Grid GG = Cycloid::Grid(5, 5, 3, 3, 1, 1);
    {
        GG.setColor({0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
        GG.setWidth(1.5, 1);
        GG.Finish();
    }
    // GG.Uniform3f("offset", {10.f, 10.f, 10.f});
    //  GG.Uniform3f("offset", {10, 10, 10});
    glm::vec3 tmp = {0, 0, -1.1};
    glUseProgram(GG.ProgramID);
    glUniform3fv(0, 1, glm::value_ptr(tmp));
    Cycloid::LineFunction::LineDescription LD([](float t) -> glm::vec3 { return {t, sin(3 * t), 0}; }, 0, M_PI, 1000);
    Cycloid::ChartFunction ChartFunc(LD, Cycloid::ColorMode::CM_PERPOSITION);
    auto kk = [](float t) -> const glm::vec4 { return {1, 1, 1, 1}; };
    Cycloid::LineFunction LineFunc(LD, Cycloid::ColorMode::CM_SINGLE, -1, 0, kk);
    Camera Cam = Camera("Proj", "View", NULL);

    LD = Cycloid::LineFunction::LineDescription([](float t) -> glm::vec3 { return {t, sin(2*t), 0}; }, 0, M_PI, 1000);
    ChartFunc.UpdateFunction(LD);
    LineFunc.UpdateFunction(LD);
    // ChartFunc.UpdateFunction(LD);
    glm::vec4 p[2] = {{0.1, 1, 1, 1}, {.2, 1.2, 1.2, 1}};

    {
        glm::mat4 projection_mat = glm::orthoRH(-1, 1, -1, 1, -10, 10);
        projection_mat[2][2] = -0.01;
        Cam.SetPositions({1, 1, 1}, {0, 0, 0});
        Cam.SetViewMat(glm::identity<glm::mat4>());
        Cam.SetProjectionMat(projection_mat);
        // Cam.SetPerspective(M_PI_2);
        Cam.AttachProgram(GG);
        Cam.AttachProgram(LineFunc);
        Cam.AttachProgram(ChartFunc);
        Cam.Update(CU_PROJ_VIEW);
    }
    auto k = Cam.GetCurrentMat();
    auto a = k * p[0], b = k * p[1];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << k[j][i] << '\t';
        }
        cout << endl;
    }
    cout << a << '\t' << b << endl;
    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        // UpdateOnChange(Cam);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GG.Draw();
        LineFunc.Draw();
        ChartFunc.Draw();
        glfwSwapBuffers(GL::window);
    }
    glfwTerminate();
    return 0;
}