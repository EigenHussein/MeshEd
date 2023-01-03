#ifndef GUIS_HH
#define GUIS_HH
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

namespace GUI
{
    void SampleWindow(bool& show_demo, ImVec4& clear_color) {
        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo);      // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

        }
    }

    bool ColorPicker(ImVec4& color) {
        ImGui::Begin("Pick a color");
        ImGui::Text("Color Selection");
        bool color_change = ImGui::ColorPicker4("Object color", (float*)&color, ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayHSV);
        //ImGui::End();
        return color_change;
    }
    bool ColorPicker2(ImVec4& color, int& vertex_number) {
        ImGui::Begin("Pick a color");
        ImGui::Text("Color Selection");
        // if (ImGui::Button("Previous"))vertex_number = (vertex_number + 3) % vertex_number;
        // ImGui::SameLine();
        // if (ImGui::Button("Next"))vertex_number = (vertex_number + 1) % vertex_number;

        bool color_change = ImGui::ColorPicker4("Object color", (float*)&color, ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayHSV);
        //ImGui::End();
        return color_change;
    }
    void StartImGui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void EndImGui() {
        ImGui::End();
    }
    void RenderImGui() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    void ShutdownImGui() {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
    }
} // namespace GUI


#endif
