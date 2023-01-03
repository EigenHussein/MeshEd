#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

// #include <VirtualScreen.h>
// #include <Fields/ComputeShader.h>
// #include <MeshGenerator.h>
// #include <dynamic_compute.h>

//#include <CycloidGraphs.h>

#include <Cycloid.h>

using namespace std;

const int width = 1000, height = 1000;

float sigmoid(const float t) {
    return 1.f / (1 + exp(t));
}
const float sph_r(const float phi, const float theta) {
    return 10 * sin(10 * phi) / (10 * phi + 0.0001);// + sin(4 * theta) / (4 * theta + 0.0001);
}
template < const float (*sphericalfunc)(const float, const float)>
const glm::vec3 sphe2cart(const glm::vec2& s) {
    const float r = sphericalfunc(s[0], s[1]);
    return {
        r * sin(s[0]) * cos(s[1]),r * sin(s[0]) * sin(s[1]),r * cos(s[0])
    };
}

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
        ), Cycloid::LineFunction::CM_PERVERTEX, -1, NULL, [](float t)->const glm::vec4 {return glm::vec4(pow(sin(3 * t), 2), pow(cos(1.5 * t), 2), abs(cos(t)), 1);}
    );

    Cycloid::Graph3D GF(
        Cycloid::Graph3D::SurfaceDescription(
            [](const glm::vec2& uv)->const glm::vec3 {
                return { uv[0],uv[1],2.1 + cos(uv[0]) + sin(uv[1]) };
            }, glm::vec2(-5.f, -5.f), glm::vec2(5.f, 5.f), glm::ivec2(100, 100)
                ), Cycloid::ColorMode::CM_PERVERTEX, -1, NULL,
        [](const glm::vec2& s)->const glm::vec4 {
                return glm::vec4(sigmoid(s.x), sigmoid(s.y), sigmoid(s.x + s.y), 1);
            }
            );
    GF.Use();
    GF.Uniform1ui("ColorMode", Cycloid::ColorMode::CM_PERPOSITION);

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
    Cycloid::FunctionSweep sweep2(
        &GF,

        Cycloid::Graph3D::SurfaceDescription(
            [](const glm::vec2& uv)->const glm::vec3 {
                return { uv[0],uv[1],1 };
            }, glm::vec2(-5.f, -5.f), glm::vec2(5.f, 5.f), glm::ivec2(100, 100)
                )

        ,
                Cycloid::Graph3D::SurfaceDescription(
                    sphe2cart<sph_r>
                    , glm::vec2(0, 0), glm::vec2(M_PI, 2 * M_PI), glm::ivec2(100, 100)
                )
                );

    Camera Cam("Proj", "View", NULL);

    Cam.AttachProgram(Func);
    Cam.AttachProgram(GG);
    Cam.AttachProgram(GF);
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
        sweep1.lerp(0.5 + 0.5 * sin(glfwGetTime()));
        sweep2.lerp(0.5 + 0.5 * sin(glfwGetTime()));
        GG.Draw();
        Func.Draw2(3);
        GF.Draw();
        T(2);
        hehe += GT(1, 2);
        glfwSwapBuffers(GL::window);
    }
    cout << hehe / float(counter) << endl;

    glfwTerminate();

    return 0;
}
