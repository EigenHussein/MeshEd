
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>
#define _USE_TIMERS
#include <complex>
#include <gl_init.h>
// Latex
#include "wavelets.h"
#include <EqTexGL.h>
#include <SVGParser.h>

using namespace std;

const int width = 1000, height = 1000;
#define IDENTITY glm::identity<glm::mat4>()

void Wavelet(std::string &dest) {
    char buf[512] = {};
    for (int i = 0; i < 250; i++) {
        float t = -5 + 10.f * i / float(249.f);
        const char *mask = t >= 0 ? "f(%.4f)=e^{-%.4f^2/b} e^{j%.4fw_{0}}" : "f(%.4f)=e^{-(%.4f)^2/b} e^{-j%.4fw_{0}}";
        snprintf(buf, 512, mask, t, t, abs(t));
        dest += std::string(buf) + std::string((i != 249) ? "\\\\\n" : "");
    }
}

int main() {

    /** Initialize GL **/
    GL::Core Core = GL::Core(
        "Grapher", {width, height}, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },
        [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }, GLFW_OPENGL_COMPAT_PROFILE);
    /** Print info **/
    print_spec();

    /**  Create Grid  **/
    Cycloid::Grid GG(20, 20, 3, 3);
    {
        GG.setColor({0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
        GG.setWidth(1.5, 1);
        GG.Finish();
    }
    auto function = CPXt_to_xyz<NormalizeWavelet<float, CPX, mother_wavelet>>;

    // /** Create Line function **/
    Cycloid::LineFunction Func(Cycloid::LineFunction::LineDescription(function, -5, 5, 2000), Cycloid::CM_PERPOSITION);

    /** Create camera **/
    Camera Cam("Proj", "View", NULL);
    // CameraPosition = {-0.01, -1, 1};
    {
        Cam.SetPositions({-0.01, -1, 1}, {0, 0, 0});
        Cam.SetPerspective(M_PI_2);
        Cam.AttachProgram(Func);
        Cam.AttachProgram(GG);
        Cam.Update(CU_PROJ_VIEW);
    }

    // Cam.Update();

    // *** LaTex *** //

    T(0);
    // auto k = Cycloid::EquationGenerator::GenerateSVG(Wavelet);
    rapidxml::xml_document<> doc;

    auto ks = Cycloid::EquationRenderer(doc, "./tmp/tmpwrt.svg", {125, 125}, 13);
    {
        ks.InitGraphics(IDENTITY, IDENTITY);
        ks.Update(CU_PROJ_VIEW);
        ks.Translate({-1, 0.9, 0});
    }

    Bezier<glm::vec3> CameraPath(
        {{3, 3, 1.5}, {1, 1, 1}, {1, 0.01, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}});

    /** Main loop **/
    static int number_of_samples = 10;
    static int counter = 0;
    static int cam_motion_counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        static bool automotion = false;
        glfwPollEvents();

        if (!automotion) {
            if (new_camera_position) {
                Cam.SetPositions(CameraPosition, {});
                Cam.Update();
            }
            cam_motion_counter = 0;
        } else {
            Cam.SetPositions(CameraPath.eval(fmod(cam_motion_counter++ / 1000.f, 1.f)), {});
            Cam.Update();
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GUI::StartImGui();
        // ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
        // ImGui::InputText("Enter", buf, 131);
        // bool isNewSample = ImGui::SliderInt("Number of samples:", &number_of_samples, 10, 2000);
       ImGui::Checkbox("Automotion", &automotion);
        bool change_frequency = ImGui::SliderFloat("f", &freq, 1, 10, "%.3f");
        bool change_b = ImGui::SliderFloat("b", &b, -10, 10, "%.3f");
        bool change_a = ImGui::SliderFloat("a", &a, 0.1, 10, "%.3f");
        if (change_b || change_a || change_frequency) {
            Func.UpdateFunction(Cycloid::LineFunction::LineDescription(function, -5, 5, 2000));
        }
        GG.Draw();
        Func.Draw2(3);
        ks.Draw((counter++ / 10) % 250);
        GUI::RenderImGui();

        glfwSwapBuffers(GL::window);
    }
    GUI::ShutdownImGui();
    glfwTerminate();

    return 0;
}
