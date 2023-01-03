
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>
#define _USE_TIMERS
#include <gl_init.h>

using namespace std;

const int width = 1000, height = 1000;

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

glm::vec3 function(const float t) {
    float sum = 0;
    for (int i = 0; i < NSAMPLES; i++) {
        sum += vals[i] * sinc(w * (t - i * dt)); /// M_PI;
    }
    return {t, sum, 0};
}

class Simulation {
  private:
    float mdt;
    int mCapacity;
    int mNSamples;
    float mTmax;
    glm::vec4 *mBuf;
    glm::vec4 *mRecon;
    bool mInitialized;

  public:
    typedef glm::vec2 (*R1R2)(const float);
    Simulation(int Cap) {
        mCapacity = Cap;
        mBuf = (glm::vec4 *)calloc(Cap, sizeof(glm::vec4));
        mRecon = (glm::vec4 *)calloc(Cap, sizeof(glm::vec4));
        mInitialized = true;
    }
    ~Simulation() {
        if (mInitialized == true) {
            free(mBuf);
            free(mRecon);
        }
    }

    const glm::vec4 *getBuf() const {
        return mBuf;
    }
    const int getNsamples() const {
        return mNSamples;
    }
    const int getCapcity() const {
        return mCapacity;
    }
    const glm::vec4 *getSincBuf() const {
        return mRecon;
    }
    void setXt(R1R2 function, float tmax, int samples) {
        mTmax = tmax;

        mNSamples = samples = min(samples, mCapacity);
        for (int i = 0; i < samples; ++i) {
            float t = tmax * float(i) / float(samples - 1);
            mBuf[i] = {function(t), t / 10.f, 0.f};
        }
    }
    void setSinc() {
        float dt = mTmax / float(mNSamples);
        float dt2 = mTmax / float(mCapacity);

        for (int n = 0; n < mCapacity; n++) {
            mRecon[n] = {n * dt2, 0, 0, 0};
            for (int i = 0; i < mNSamples; i++) {
                mRecon[n].y += mBuf[i].y * sinc((dt2 * n - i * dt) / dt);
            }
        }
    }
};

int main() {

    /** Initialize GL **/
    GL::Core Core = GL::Core(
        "Grapher", {width, height}, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },
        [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; });
    // glDisable(GL_DEPTH_TEST);
    /** Print info **/
    print_spec();

    /**  Create Grid  **/
    Cycloid::Grid GG(5, 5, 3, 3);
    // GG.setColor(0x7fffffff, 0x7f70ff7f); //{0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
    GG.setColor({0.5, 1, 1, 1}, {0.5, 0.8, 1, 0.5});
    GG.setWidth(1.5, 1);
    GG.Finish();

    /** Create Line function **/
    Cycloid::LineFunction Func(
        Cycloid::LineFunction::LineDescription(
            //[](const float t) -> glm::vec3 { return glm::vec3(cos(t), sin(t), t) * 0.5f + glm::vec3(0.5); }, 0,
            function, 0, 1, 10000),
        Cycloid::CM_PERPOSITION);

    /** Create camera **/
    Camera Cam("Proj", "View", NULL);
    // CameraPosition = {-0.01, -1, 1};
    bool is3D = true;
    if (is3D) {
        Cam.SetPositions({-0.01, -1, 1}, {0, 0, 0});

        Cam.SetPerspective(M_PI_2);
        glEnable(GL_DEPTH_TEST);
    } else {
        Cam.SetViewMat(glm::identity<glm::mat4>());
        Cam.SetProjectionMat(glm::identity<glm::mat4>());
        glDisable(GL_DEPTH_TEST);
    }
    Cam.AttachProgram(Func);
    Cam.AttachProgram(GG);

    Cam.Update(CU_PROJ_VIEW);
    // Cam.Update();

    Simulation Sim(10000);

    /** Main loop **/
    static int number_of_samples = 10;
    while (!glfwWindowShouldClose(GL::window)) {

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GUI::StartImGui();
        char buf[131] = {};
        static bool kek;
        // ImGui::InputText("Enter", buf, 131);
        bool isNewSample = ImGui::SliderInt("Number of samples:", &number_of_samples, 10, 2000);
        ImGui::SameLine();
        static bool isSincInter;
        kek = ImGui::Checkbox("SincInterpolation", &isSincInter);

        if (isNewSample) {
            Sim.setXt([](const float t) -> glm::vec2 { return {t, sin(2.01 * M_PI * 4 * t)}; }, 1, number_of_samples);
        }
        if (kek) {
            if (isSincInter) {
                Sim.setSinc();
                Func.SceneData->UpdateBuffer(0, Sim.getCapcity() * sizeof(glm::vec4), Sim.getSincBuf());
            } else {
                Func.SceneData->UpdateBuffer(0, Sim.getNsamples() * sizeof(glm::vec4), Sim.getBuf());
            }
        }
        GG.Draw();
        Func.Draw2(3, isSincInter ? Sim.getCapcity() : Sim.getNsamples());
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }
    GUI::ShutdownImGui();
    glfwTerminate();

    return 0;
}
