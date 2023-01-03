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
#include <memory>
#include <memory.h>
using namespace std;

const int width = 1000, height = 1000;
#define IDENTITY glm::identity<glm::mat4>()

typedef std::complex<float> CPX;
#define J CPX(0, 1)

static float freq = 1, b = 1;
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

template <typename _T> std::unique_ptr<_T> CYCLOID_UNIQUE(_T *ptr) {
    return std::unique_ptr<_T>(ptr);
}
rapidxml::xml_document<> doc;

class Default_Prog {
  private:
    unique_ptr<GL::Core> Core;
    unique_ptr<Cycloid::Grid> Grid;
    unique_ptr<Camera> Cam;
    unique_ptr<Cycloid::LineFunction> Func;
    unique_ptr<Cycloid::EquationRenderer> SVGRenderer;
    unique_ptr<Bezier<glm::vec3>> BezCurve;

    void InitGL() {

        /** Initialize GL **/
        Core = CYCLOID_UNIQUE(new GL::Core(
            "Grapher", {width, height}, GL::Core::InitType::IT_OpenGL_ImGui,
            [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },
            [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }, GLFW_OPENGL_COMPAT_PROFILE));
        // glDisable(GL_DEPTH_TEST);
        /** Print info **/
        print_spec();
    }
    void InitGrid() {
        Grid = CYCLOID_UNIQUE(new Cycloid::Grid(20, 20, 3, 3));
        // GG.setColor(0x7fffffff, 0x7f70ff7f); //{0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
        Grid->setColor({0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
        Grid->setWidth(1.5, 1);
        Grid->Finish();
    }
    void InitFunction() {
        // /** Create Line function **/
        Func = CYCLOID_UNIQUE(new Cycloid::LineFunction(Cycloid::LineFunction::LineDescription(function, -5, 5,
        2000),
                                                        Cycloid::CM_PERPOSITION));
    }
    void InitCamera() {
        Cam = CYCLOID_UNIQUE(new Camera("Proj", "View", NULL));
        // CameraPosition = {-0.01, -1, 1};

        Cam->SetPositions({-0.01, -1, 1}, {0, 0, 0});
        Cam->SetPerspective(M_PI_2);
        glEnable(GL_DEPTH_TEST);

        Cam->AttachProgram(*Func);
        Cam->AttachProgram(*Grid);

        Cam->Update(CU_PROJ_VIEW);
        // Cam.Update();

        // *** LaTex *** //
    }
    void InitOthers() {

        SVGRenderer = CYCLOID_UNIQUE(new Cycloid::EquationRenderer(doc, "./tmp/tmpwrt.svg", {125, 125}, 13));
        SVGRenderer->InitGraphics(IDENTITY, IDENTITY);
        SVGRenderer->Update(CU_PROJ_VIEW);
        SVGRenderer->Translate({-1, 0.9, 0});
        BezCurve = CYCLOID_UNIQUE(new Bezier<glm::vec3>(
            {{3, 3, 1.5}, {1, 1, 1}, {1, 0.01, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}, {-0.01, -1, 1}}));
    }

  public:
    void DrawFrame() {
        static int counter = 0;
        glfwPollEvents();
        // if (new_camera_position) {
        //     Cam.SetPositions(CameraPosition, {});
        //     Cam.Update();
        // }
        Cam->SetPositions(BezCurve->eval(fmod(glfwGetTime() / 10.f, 1.f)), {});
        Cam->Update();

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
            Func->UpdateFunction(Cycloid::LineFunction::LineDescription(function, -5, 5, 2000));
        }
        Grid->Draw();
        Func->Draw2(3);
        SVGRenderer->Draw((counter++ / 10) % 250);
        GUI::RenderImGui();

        glfwSwapBuffers(GL::window);
    }
    Default_Prog() {
        InitGL();
        InitGrid();
        InitFunction();
        InitCamera();
        InitOthers();

    }
    ~Default_Prog() {
        GUI::ShutdownImGui();
        // glfwTerminate(); /*  */
    }
};

int main() {

    Default_Prog a;
    while (!glfwWindowShouldClose(GL::window)) {
        a.DrawFrame();
    }
}
