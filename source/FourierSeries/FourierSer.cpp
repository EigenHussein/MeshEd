
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>
#define _USE_TIMERS
#include <complex>
#include <gl_init.h>
// Latex

#include "FourierUtils.h"
#include <EqTexGL.h>
#include <SVGParser.h>

using namespace std;

const int width = 1000, height = 1000;
#define IDENTITY glm::identity<glm::mat4>()

#include <fftw3.h>

std::ostream &operator<<(std::ostream &o, const fftw_complex &cpx) {
    o << "[" << cpx[0] << ',' << cpx[1] << "]";
    return o;
}

std::ostream &operator<<(std::ostream &o, const fftwf_complex &cpx) {
    o << "[" << cpx[0] << ',' << cpx[1] << "]";
    return o;
}

void getFourier(std::vector<glm::vec4> &points, fftwf_complex *cn, int len, int stride, int count,
                Cycloid::LineFunction::LineDescription range, int freq = 1) {

    points.reserve(range.steps * (count + 1));
    for (int i = 0; i < range.steps * (count + 1); i++) {
        points.push_back({});
    }
    cout << points.data() << endl;
    float dt = (range.b - range.a) / float(range.steps);
    glm::vec2 *ptr = (glm::vec2 *)cn;
    *(ptr++) /= float(len);
    for (int i = 1; i < len; i++) {
        *(ptr++) /= float(len) / 2.f;
    }

    ptr = (glm::vec2 *)cn;
    glm::vec4 *last_func = points.data() + range.steps * (count);
    for (int eq = 0; eq < count; eq++) {
        glm::vec4 *current_func = points.data() + range.steps * eq;
        for (int i = 0; i < range.steps; i++) {

            float t = range.a + dt * i;
            if (eq == 0) {
                last_func[i] = {0, t, cn[0][0], 0};
            }
            int idx = stride * eq + 1;
            float val =
                cn[idx][0] * cos(2 * M_PI * (eq + 1) * freq * t) + cn[idx][1] * sin(2 * M_PI * (eq + 1) * freq * t);
            current_func[i] = {0, t, val, 0};
            last_func[i].z += val;
        }
    }
    cout << points.data() << endl;
    cout << points.size() << endl;
}

int main() {

    FFTClass<float> FFT;
    fftwf_complex *input = (fftwf_complex *)FFT.getInputBuffer();
    fftwf_complex *output = (fftwf_complex *)FFT.getOutputBuffer();

    for (int i = 0; i < 1024; i++) {
        int k = (i + 256) % 1024;
        input[i][0] = (k < 700) ? 1.f : 0.0;
        input[i][1] = 0.0;
    }

    FFT.Perform();

    std::vector<glm::vec4> FourierDecomp;
    getFourier(FourierDecomp, output, 1024, 1, 10, {NULL, -5, 5, 1000}, 1);

    cout << FourierDecomp.size() << endl;

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

    // /** Create Line function **/
    auto function = [](const float t) -> glm::vec3 { return {t, t, t}; };
    Cycloid::LineFunction Func(Cycloid::LineFunction::LineDescription(NULL, -5, 5, FourierDecomp.size()),
                               Cycloid::CM_PERPOSITION);

    auto GPUBuf = Func.GetVBO();
    glNamedBufferSubData(GPUBuf.buf_idx, 0, sizeof(glm::vec4) * FourierDecomp.size(), FourierDecomp.data());

    std::vector<Cycloid::LineFunction::SubFunction> func; //= {{0, 1000, {}} /* , {1000, 1000, {0, 0, 1}} */};
    func.emplace_back(10000, 1000, glm::vec3(1, 0.f, 0.f));
    for (int i = 0; i < 10; i++) {
        func.emplace_back(i * 1000, 1000, glm::vec3(-float(i), 0.f, 0.f));
    }

    /** Create camera **/
    Camera Cam("Proj", "View", NULL);
    // CameraPosition = {-0.01, -1, 1};
    {
        Cam.SetPositions({-0.01, -1, 1}, {0, 0, 0});
        Cam.SetPerspective(M_PI_2);
        // Cam.SetOrtho(-2.f, 2.f, -2.f, 2.f);
        Cam.AttachProgram(Func);
        Cam.AttachProgram(GG);
        Cam.Update(CU_PROJ_VIEW);
    }

    while (!glfwWindowShouldClose(GL::window)) {
        static bool automotion = false;
        glfwPollEvents();

        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GG.Draw();
        // Func.Draw2(3);
        Func.DrawMulFunc(func, 2);
        glfwSwapBuffers(GL::window);
    }

    return 0;
}