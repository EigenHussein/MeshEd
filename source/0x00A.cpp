#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
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

class DynamicVF {
private:
    Model Arrow;
    GLuint NormalMap;
    ComputeShader InitShader;
    ComputeShader StepShader;
    glm::ivec3 BaseCount;
    glm::ivec3 interCount;
    ModelExplorer Mex;


public:
    Camera Cam;
    DynamicVF(glm::ivec3 Base, glm::ivec3 Interpolated) :
        BaseCount(Base),
        interCount(Interpolated),
        Arrow(Model("/home/hussein/3D Models/Arrows.obj")),
        NormalMap(VirtualScreen::LoadTexture("/home/hussein/3D Models/Arrows.png")),
        InitShader(ComputeShader("../shaders/0x00A/compute_field_init.glsl")),
        StepShader(ComputeShader("../shaders/0x00A/compute_field_inter.glsl")),
        Mex(ModelExplorer(Arrow, GL::GetProgram(
            "../shaders/0x008/vertex_compute.glsl",
            "../shaders/0x008/fragment_nmap.glsl", NULL))),
        Cam(Camera("Proj", "View", NULL))
    {


        InitShader.AddSSBO(interCount.x * interCount.y * interCount.z * sizeof(ModelMatrices), 0);
        InitShader.AddSSBO(BaseCount.x * BaseCount.y * BaseCount.z * sizeof(VectorField), 1);
        //Base
        glUseProgram(InitShader.ProgramID);
        glDispatchCompute(BaseCount.x / 2, BaseCount.y / 2, BaseCount.z / 2);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ALL_BARRIER_BITS);
        //RefreshBaseField();


        Cam.AttachProgram(Mex);
        Cam.SetPositions({ 2,2,2 }, { 0,0,0 });
        Cam.SetPerspective(M_PI_2);
        Cam.Update(CU_PROJ_VIEW);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NormalMap);
        glfwSwapInterval(2);
        int counter = 0;
        Init_Interp();
        glUseProgram(StepShader.ProgramID);
        glUniform1f(glGetUniformLocation(StepShader.ProgramID, "a"), 50);
        glUniform1f(glGetUniformLocation(StepShader.ProgramID, "b"), -1);
        glDispatchCompute(interCount.x / 8, interCount.y / 8, interCount.z / 8);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //MoveField();
    }
    void RefreshBaseField() {
        glUseProgram(InitShader.ProgramID);
        glDispatchCompute(BaseCount.x / 2, BaseCount.y / 2, BaseCount.z / 2);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    void MoveField() {

        glUseProgram(StepShader.ProgramID);
        glDispatchCompute(interCount.x / 8, interCount.y / 8, interCount.z / 8);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    void NextFrame() {
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, ObjectPosition);
            Cam.Update();
            //new_camera_position = false;
        }
        //MyMesh.Draw();

        glUseProgram(Mex.ProgramID);
        if (new_camera_position) {
            glUniform3fv(glGetUniformLocation(Mex.ProgramID, "CameraPosition"), 1, glm::value_ptr(CameraPosition));
            new_camera_position = false;
        }
        glBindVertexArray(Mex.SceneData->VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mex.SceneData->EBO);
        glDrawElementsInstanced(GL_TRIANGLES, Arrow.nIndices(), GL_UNSIGNED_INT, 0, interCount.x * interCount.y * interCount.z);
    }
    void Init_Interp() {
        ModelMatrices* Interp = new ModelMatrices[interCount.x * interCount.y * interCount.z];
        auto ptr = Interp;
        for (int i = 0;i < interCount.x;i++) {
            for (int j = 0;j < interCount.y;j++) {
                for (int k = 0;k < interCount.z;k++) {
                    ptr->Position = glm::vec4(-8 + i, -8 + j, -8 + k, 0);
                    ptr++;
                }
            }
        }
        InitShader.SetData(0, Interp, interCount.x * interCount.y * interCount.z * sizeof(ModelMatrices), 0);
        delete[] Interp;
    }
    ComputeShader* Shader() {
        return &InitShader;
    }

};




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


    DynamicVF DynVF({ 64,64,64 }, { 16,16,16 });
    WireRenderer WR(DynVF.Cam);
    // ModelMatrices Interp[16][16][16];
    // for (int i = 0;i < 16;i++) {
    //     for (int j = 0;j < 16;j++) {
    //         for (int k = 0;k < 16;k++) {
    //             Interp[i][j][k].Position = glm::vec4(-8 + i, -8 + j, -8 + k, 0);
    //         }
    //     }
    // }
    //DynVF.Shader()->SetData(0, Interp, sizeof(Interp), 0);

    auto ShaderUpdateFunction = [&](ComputeShader* cshader, const char* func) {
        std::string csource = P0x00A_Dyn;
        csource += func + string(";\n}");
        if (cshader != NULL) {
            return cshader->UpdateProgram(csource.c_str());
        }
    };

    // unsigned int buffy[1000] = {};

    // glGetNamedBufferSubData(WR.WireMesh->SceneData->EBO, 0, sizeof(buffy), buffy);

    // for (int i = 0;i < 1000;i++) {
    //     cout << i << '\t' << buffy[i] << '\n';
    // }


    //return 0;


    glPointSize(3);
    float k = glfwGetTime();
    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        glClearColor(0.1, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        WR.draw();
        // glUseProgram(WR.getProgramID());
        // glBindVertexArray(WR.WireMesh->SceneData->VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WR.WireMesh->SceneData->EBO);
        // glDrawElements(GL_TRIANGLES, glfwGetTime() - k, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, 10*(glfwGetTime() - k));

        DynVF.MoveField();
        DynVF.NextFrame();

        gui_wire_function();
        if (wire_function_update) {
            ShaderUpdateFunction(DynVF.Shader(), wire_function);
            DynVF.RefreshBaseField();
        }
        else if (reset_request) {
            DynVF.Init_Interp();
        }
        // else if (radius_slider_update) {
        //     glUseProgram(Wire.ProgramID);
        //     glUniform1f(glGetUniformLocation(Wire.ProgramID, "radius_cs"), radius_cs);
        //     UpdateWire();
        // }
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }

    glfwTerminate();

    return 0;
}

