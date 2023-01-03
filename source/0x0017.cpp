
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

using namespace std;

const int width = 1000, height = 1000;
#define IDENTITY glm::identity<glm::mat4>()

float sigmoid(const float t) {
    return 1.f / (1 + exp(t));
}

const float sph_r(const float phi, const float theta) {
    return 10 * sin(10 * phi) / (10 * phi + 0.0001); // + sin(4 * theta) / (4 * theta + 0.0001);
}

const int NSAMPLES = 100;
float vals[NSAMPLES];
const float dt = 1.0f / float(NSAMPLES);
const float w = 2 * M_PI / (dt);

template <typename _T> _T sinc(const _T x) {
    return sin(M_PI * x) / (0.000001 + M_PI * x);
}

typedef std::complex<float> CPX;
static float freq = 1, b = 1;
#define J CPX(0, 1)
CPX ct_function(float t) {
    return exp(J * 2.f * float(M_PI) * freq * t) * exp(-t * t / b);
}
glm::vec3 function(float t) {
    auto tmp = ct_function(t);
    return {t, tmp.real(), tmp.imag()};
}

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
    glDisable(GL_DEPTH_TEST);
    /** Print info **/
    print_spec();

    /**  Create Grid  **/
    Cycloid::Grid GG(20, 20, 3, 3);
    // GG.setColor(0x7fffffff, 0x7f70ff7f); //{0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
    GG.setColor({0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
    GG.setWidth(1.5, 1);
    GG.Finish();

    // /** Create Line function **/
    Cycloid::LineFunction Func(Cycloid::LineFunction::LineDescription(function, -5, 5, 2000), Cycloid::CM_PERPOSITION);

    /** Create camera **/
    Camera Cam("Proj", "View", NULL);
    // CameraPosition = {-0.01, -1, 1};
    bool is3D = true;
    if (is3D) {
        Cam.SetPositions({-0.01, -1, 1}, {0, 0, 0});
        Cam.SetPerspective(M_PI_2);
        glEnable(GL_DEPTH_TEST);
    } else {
        Cam.SetViewMat(IDENTITY);
        Cam.SetProjectionMat(IDENTITY);
        glDisable(GL_DEPTH_TEST);
    }
    Cam.AttachProgram(Func);
    Cam.AttachProgram(GG);

    Cam.Update(CU_PROJ_VIEW);
    // Cam.Update();

    // *** LaTex *** //

    T(0);
    // auto k = Cycloid::EquationGenerator::GenerateSVG(Wavelet);
    rapidxml::xml_document<> doc;

    auto ks = Cycloid::EquationRenderer(doc, "./tmp/tmpwrt.svg", {125, 125}, 13);
    ks.InitGraphics(IDENTITY, IDENTITY);
    ks.Update(CU_PROJ_VIEW);
    ks.Translate({-1, 0.9, 0});

    Bezier<glm::vec3> CameraPath(
        {{3, 3, 1.5}, {1, 1, 1}, {1, 0.01, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}});

    /** Main loop **/
    static int number_of_samples = 10;
    static int counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }
        Cam.SetPositions(CameraPath.eval(fmod(glfwGetTime() / 10.f, 1.f)), {});
        Cam.Update();

        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GUI::StartImGui();
        // ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
        // ImGui::InputText("Enter", buf, 131);
        // bool isNewSample = ImGui::SliderInt("Number of samples:", &number_of_samples, 10, 2000);
        bool change_frequency = ImGui::SliderFloat("f", &freq, 1, 10, "%.3f");
        bool change_b = ImGui::SliderFloat("b", &b, 1, 10, "%.3f");
        if (change_b || change_frequency) {
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
