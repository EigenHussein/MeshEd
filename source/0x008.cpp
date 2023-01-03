#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <VirtualScreen.h>
#include <Fields/ComputeShader.h>
#include <MeshGenerator.h>

using namespace std;

const unsigned int width = 1024, height = 1024;
#define FieldSize 32
#define InterSize 16

#define RECs

glm::vec3 PathFunction(float t) {
    return 5.f * glm::vec3(cos(6 * 1.1 * t) + 0.0001, sin(6 * 1.1 * t) + 0.0001, t / 5);
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


    auto loop = MeshGenerator::Shapes2D::Circle(100);
    auto path = MeshGenerator::GetParametricCurve2(PathFunction, 0, 2 * M_PI, 300);
    MeshGenerator::CPUMeshGenerator MESH = MeshGenerator::CPUMeshGenerator(path, loop);
    EMesh MyMesh = EMesh(MESH);


    Model Arrow = Model("/home/hussein/3D Models/Arrows.obj");
    GLuint NormalMap = VirtualScreen::LoadTexture("/home/hussein/3D Models/Arrows.png");

    auto a = std::chrono::high_resolution_clock::now();
    ComputeShader Compute("../shaders/0x008/compute_init.glsl");
    Compute.AddSSBO(InterSize * InterSize * InterSize * sizeof(ModelMatrices));
    Compute.AddSSBO(FieldSize * FieldSize * FieldSize * sizeof(VectorField));
    glUseProgram(Compute.ProgramID);
    glDispatchCompute(FieldSize / 8, FieldSize / 8, FieldSize / 8);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    auto b = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(b - a).count() << std::endl;

    ComputeShader Compute2("../shaders/0x008/compute_interpolate.glsl");
    glUseProgram(Compute2.ProgramID);

    glUniform1f(glGetUniformLocation(Compute2.ProgramID, "a"), 50);
    glUniform1f(glGetUniformLocation(Compute2.ProgramID, "b"), -1);
    glDeleteShader(Compute.ProgramID);

    ModelExplorer Mex = ModelExplorer(Arrow, GL::GetProgram(
        "../shaders/0x008/vertex_compute.glsl",
        "../shaders/0x008/fragment_nmap.glsl", NULL));

    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(MyMesh);
    Cam.AttachProgram(Mex);
    Cam.SetPositions({ 2,2,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, NormalMap);
    glfwSwapInterval(2);
    int counter = 0;

    glUseProgram(Compute2.ProgramID);
    glDispatchCompute(InterSize / 8, InterSize / 8, InterSize / 8);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, ObjectPosition);
            Cam.Update();
            //new_camera_position = false;
        }
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*         auto a = std::chrono::high_resolution_clock::now(); */
        // glUseProgram(Compute2.ProgramID);
        // glDispatchCompute(BFS / 8, BFS / 8, BFS / 8);
        // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        /*       auto b = std::chrono::high_resolution_clock::now();
              std::cout << std::chrono::duration_cast<std::chrono::microseconds>(b - a).count() << std::endl; */

        //MyMesh.Draw();

        glUseProgram(Mex.ProgramID);
        if (new_camera_position) {
            glUniform3fv(glGetUniformLocation(Mex.ProgramID, "CameraPosition"), 1, glm::value_ptr(CameraPosition));
            new_camera_position = false;
        }
        glBindVertexArray(Mex.SceneData->VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mex.SceneData->EBO);

        glDrawElementsInstanced(GL_TRIANGLES, Arrow.nIndices(), GL_UNSIGNED_INT, 0, 16 * 16 * 16);

        glfwSwapBuffers(GL::window);
    }

    glfwTerminate();

    return 0;
}

