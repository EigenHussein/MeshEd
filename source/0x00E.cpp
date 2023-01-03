#define _USE_TIMERS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <Cycloid.h>

using namespace std;

const int width = 1000, height = 1000;

void print_spec() {
    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    cout << vendor << '\t' << renderer << endl;
}

int main() {

    GL::Core Core("Grapher", { width,height }, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow* wnd) {
            glfwSetKeyCallback(wnd, KeyCallback);
        }
        ,
            [](ImGuiIO* ptr) {
            ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }
        );

    print_spec();
    Cycloid::Grid GG(5, 5, 3, 3);
    GG.setColor({ 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,1 }, { 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,0.5 });
    GG.setWidth(1.5, 1);
    GG.Finish();

    Cycloid::LineFunction Func(
        Cycloid::LineFunction::LineDescription(
            [](float t)->glm::vec3 {return { cos(t),sin(t),0 };}, 0, 2 * M_PI, 1000
        ), Cycloid::LineFunction::CM_PERVERTEX, -1,
        NULL, [](float t)->const glm::vec4 {
            return (1.f / float(2 * M_PI)) * glm::vec4(t, t, t, t);
        }
    );

    Cycloid::FunctionSweep sweep1(
        &Func, {
            [](float t) {
                return glm::vec3(cos(t),sin(t),-0.001);
            },0,2 * M_PI,1000
        },
        {
            [](float t) {
                return glm::vec3((1 + 0.5 * cos(5 * t)) * cos(t),(1 + 0.5 * cos(5 * t)) * sin(t),-0.001);
            },0,2 * M_PI,1000
        }
        );
    Camera Cam("Proj", "View", NULL);

    Cam.AttachProgram(Func);
    Cam.AttachProgram(GG);
    Cam.SetPositions({ 1,1,1 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_MODEL);

    int counter = 0;
    int hehe = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        counter++;

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        T(1);
        float t = glfwGetTime();
        glm::vec3 tmp = { cos(t) * (1 + cos(t)),sin(t) * (1 + cos(t)),sin(t) };
        Func.AppendValue(&tmp, 1);
        // sweep1.lerp(1);
        GG.Draw();
        Func.Draw2(3);
        T(2);
        hehe += GT(1, 2);
        glfwSwapBuffers(GL::window);
    }
    cout << hehe / float(counter) << endl;

    glfwTerminate();

    return 0;
}

