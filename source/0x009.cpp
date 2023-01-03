#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <VirtualScreen.h>
#include <Fields/ComputeShader.h>
#include <MeshGenerator.h>
#include <dynamic_compute.h>

using namespace std;

const unsigned int width = 1024, height = 1024;
#define VFS 64
#define BFS 16

#define RECs

glm::vec3 PathFunction(float t) {
    return 5.f * glm::vec3(cos(6 * 1.1 * t) + 0.0001, sin(6 * 1.1 * t) + 0.0001, t / 5);
}

EMesh GetPipe(const int c_seg = 100, glm::vec3(*pathfunc)(float) = PathFunction) {
    auto loop = MeshGenerator::Shapes2D::Circle(c_seg);
    auto path = MeshGenerator::GetParametricCurve2(pathfunc, 0, 2 * M_PI, 300);
    MeshGenerator::CPUMeshGenerator MESH = MeshGenerator::CPUMeshGenerator(path, loop);
    return EMesh(MESH);
}




int main() {
    GL::Core Core("Meshy", { width,height }, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow* wnd) {
            glfwSetKeyCallback(wnd, KeyCallback);
        }
        ,
            [](ImGuiIO* ptr) {
            ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }
        );
    Camera Cam = Camera("Proj", "View", NULL);

    WireRenderer WR = WireRenderer(Cam);

    cout << "WR INIT" << endl;
    Cam.SetPositions({ 1,1,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);


    glfwSwapInterval(2);
    int count = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, { 0,0,0 });
            Cam.Update();
            new_camera_position = false;
        }

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gui_wire_function();
        WR.draw();

        if (wire_function_update)cout << wire_function << endl;
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }
    GUI::ShutdownImGui();
    glfwTerminate();

    return 0;
}
int main2() {

    GL::Core Core("Meshy", { width,height }, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow* wnd) {
            glfwSetKeyCallback(wnd, KeyCallback);
        }
        ,
            [](ImGuiIO* ptr) {
            ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }
        );


    // Model Arrow = Model("/home/hussein/3D Models/Arrows.obj");
    // GLuint NormalMap = VirtualScreen::LoadTexture("/home/hussein/3D Models/Arrows.png");

    const int np = 513 + 1, nc = 129 + 1;
    auto indices = MeshGenerator::CPUMeshGenerator::gen_inds(np, nc);
    // cout << indices.nIndices << endl;
    // for (int i = 0;i < 9;i++) {
    //     cout << indices.Indices[i] << '\t';
    // }

    //return 0;

    ComputeShader Wire = ComputeShader("../shaders/0x009/wire_generator.glsl");
    ComputeShader NormalGenerator = ComputeShader("../shaders/0x009/norm_generator.glsl");

    auto UpdateWire = [&]() {
        glUseProgram(Wire.ProgramID);
        glUniform1f(glGetUniformLocation(Wire.ProgramID, "radius_cs"), radius_cs);
        glDispatchCompute(np, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    };
    auto UpdateNormals = [&]() {
        glUseProgram(NormalGenerator.ProgramID);
        glDispatchCompute(np, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    };

    new_compute(&Wire, "vec3(cos(5*t),sin(5*t),t)");
    glm::vec4 cir[nc];
    for (int i = 0;i < nc;i++) {
        float t = i * 2 * M_PI / float(nc - 1);
        cir[i] = { cos(t),sin(t),0,0 };
    }

    auto CircleSSBO = Wire.AddSSBO(sizeof(cir));
    Wire.SetData(0, cir, sizeof(cir), 0);
    auto SSBO = Wire.AddSSBO(np * nc * 2 * sizeof(glm::vec4));

    UpdateWire();
    UpdateNormals();
    GLuint Prog = GL::GetProgram("../shaders/0x009/v.glsl", "../shaders/0x009/f.glsl", NULL);
    iMesh WireMesh(DT_ELEMENT, SSBO, { GLAttrib(0,3,8,0),GLAttrib(1,3,8,4) },
        (np - 1) * nc, indices.Indices, indices.nIndices - 6 * (nc - 1), GL_TRIANGLES, Prog);

    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(WireMesh);
    Cam.SetPositions({ 1,1,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);
    glPointSize(3);

    glfwSwapInterval(2);
    int count = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, { 0,0,0 });
            Cam.Update();
            new_camera_position = false;
        }
        gui_wire_function();
        if (wire_function_update) {
            new_compute(&Wire, wire_function);
            UpdateWire();
            UpdateNormals();
        }
        else if (radius_slider_update) {
            glUseProgram(Wire.ProgramID);
            glUniform1f(glGetUniformLocation(Wire.ProgramID, "radius_cs"), radius_cs);
            UpdateWire();
            UpdateNormals();
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //
        WireMesh.Draw();
        // glUseProgram(WireMesh.ProgramID);
        // glBindVertexArray(WireMesh.SceneData->VAO);
        // glDrawArrays(GL_POINTS,0,(count+=101)%40000);
        //
        if (wire_function_update)cout << wire_function << endl;
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }

    glfwTerminate();

    return 0;
}

