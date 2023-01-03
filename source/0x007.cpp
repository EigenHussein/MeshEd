#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>
using namespace std;



int main() {

    GL::Core Core("Meshy", { 1024,1024 }, GL::Core::InitType::IT_OpenGL_ImGui,
        []() {
            glfwSetKeyCallback(GL::window, KeyCallback);
        }
        ,
            [](ImGuiIO* ptr) {
            ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;/* | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoDecoration
                | ImGuiWindowFlags_AlwaysAutoResize
                | ImGuiWindowFlags_NoSavedSettings; */
        }
        );
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glm::vec3 vertices[] = {
{1,1,0},
{-1,1,0},
{-1,-1,0},
{1,-1,0}
    };
    glm::vec4 cols[4] = {
        {1,0,0,1},{1,1,0,1},{1,0,1,1},{0,1,1,1}
    };
    unsigned int indices[] = { 0,1,2,2,3,0 };
    GLuint MyProg = GL::GetProgram("../shaders/0x007/vertex.glsl", "../shaders/0x007/fragment.glsl", NULL);
    //DirectCall DC = DirectCall(vertices, 6, GL_LINE_STRIP, GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL));
    AttributedData ATD = AttributedData(
        { DataBuffer(vertices,sizeof(vertices)),DataBuffer(cols,sizeof(cols)) },
        { GLAttrib(0,3,3,0),GLAttrib(1,4,4,12) }
    );
    iMesh DC = iMesh(DT_ELEMENT, ATD, 4, indices, 6, GL_TRIANGLES, MyProg);
    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(DC);
    Cam.SetPositions({ 1,1,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(1);
    bool show_demo_window = true;
    bool show_another_window = false;
    while (!glfwWindowShouldClose(GL::window))
    {

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, { 0,0,0 });
            Cam.Update();
            new_camera_position = false;
        }
        GUI::StartImGui();


        ImGui::Begin("Hello");
        static int vert = 0;

        if (ImGui::Button("Previous")) {
            vert = (vert + 3) % 4;
        }
        ImGui::SameLine();
        if (ImGui::Button("Next")) {
            vert = (vert + 1) % 4;
        }
        ImGui::BeginGroup();

        if (ImGui::ColorPicker4("Pick Vertex Color", (float*)&clear_color)) {
            glBindBuffer(GL_ARRAY_BUFFER, DC.SceneData->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + vert * (sizeof(ImVec4)), sizeof(ImVec4), (void*)&clear_color);
        }
        ImGui::EndGroup();
        ImGui::End();
        //         // Start the Dear ImGui frame
        //         ImGui::ShowDemoWindow(&show_demo_window);
        //   //      GUI::StartImGui();
        //         //GUI::SampleWindow(show_demo_window, clear_color);

        //         static int vertex_number = 0;



                // if (GUI::ColorPicker2(clear_color, vertex_number)) {
                //     //glUniform4fv(0, 1, (float*)&clear_color);
                // }
                // if (ImGui::Button("Previous"))vertex_number = (vertex_number + 3) % vertex_number;
                // ImGui::SameLine();
                // if (ImGui::Button("Next"))vertex_number = (vertex_number + 1) % vertex_number;

        //        GUI::EndImGui();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DC.Draw();
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();

    return 0;
}


