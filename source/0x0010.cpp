#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <Cycloid.h>
#include <sock_util.h>

using namespace std;

const int width = 1000, height = 1000;
char gui_text1[4096];
float gui_float1 = 0;
bool b1, b2, b3;
vector<char*> hehe = {
    "Hello1","Hello2","Hello3","Hello4"
};
int gui_selected_element = 0;
void gui__() {
    GUI::StartImGui();
    ImGui::Begin("Wire function update");
    ImGui::ListBox("Players", &gui_selected_element, hehe.data(), 4);
    float kfek[3];
    ImGui::DragFloat3("Heheboy", kfek);
    ImGui::VSliderFloat("Keku", { 20,200 }, kfek, 0, 10);
    bool text_enter = ImGui::InputText("Line Function:", gui_text1, sizeof(gui_text1), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    bool button_click = ImGui::Button("Enter");
    ImGui::TreeNode("TreeNode");
    ImGui::TreePop();
    b1 = ImGui::SliderFloat("cross section radius", &gui_float1, 0.01, 5);
    b2 = text_enter | button_click;
    ImGui::NewLine();
    b3 = ImGui::Button("Reset");
    GUI::EndImGui();

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
    /**********************GRID*************************/
    Cycloid::Grid GG(5, 5, 3, 3);
    GG.setColor({ 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,1 }, { 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,0.5 });
    GG.setWidth(1.5, 1);
    GG.Finish();

    /**********************3D Graph*********************/


    auto Surf = Cycloid::Graph3D::SurfaceDescription(
        [](const glm::vec2& uv)-> const glm::vec3 {

            auto aha = [&](const glm::vec2& uv, const glm::vec2& ref) ->const glm::vec3 {
                const glm::vec2 tmp = uv - ref;
                float r = glm::length(tmp);

                float res = exp(-0.2 * r) * sin(5 * r);
                return { tmp[0],tmp[1],res };
            };



            return aha(uv, {}) + aha(uv, { 3,3 });
        }, { -10.f,-10.f }, { 10.f,10.f }, { 200,200 }
        );
    auto SurfColor = [](const glm::vec2& uv)->const glm::vec4 {
        glm::vec2 color = glm::vec2(5.f, 5.f) + uv;
        return { 0.1f * color.x,0.1f * color.y,0.5 * (color.x + color.y) ,1.f };
    };
    Cycloid::Graph3D G3D(Surf, Cycloid::ColorMode::CM_PERVERTEX, -1, NULL, SurfColor);

    Camera Cam("Proj", "View", NULL);
    Cam.AttachProgram(GG);
    Cam.AttachProgram(G3D);
    Cam.SetPositions({ 1,1,1 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(GL::window)) {

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, ObjectPosition);
            Cam.Update();
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GG.Draw();
        G3D.Draw();
        bool k = true;
        //GUI::StartImGui();
        //ImGui::ShowDemoWindow(&k);
        // gui__();

        //GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }
    glfwTerminate();
    GUI::ShutdownImGui();
    return 0;
}
