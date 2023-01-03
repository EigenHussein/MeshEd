#ifndef GL_INIT_HH_
#define GL_INIT_HH_
#include <Core2.h>
#include <CoreUtils/CameraControl.h>
#include <CoreUtils/MyPrograms.h>
#include <GUIs.h>
#include <Program.h>

#include <imgui.h>

#ifdef _USE_TIMERS

#define T(n) auto aka##n = std::chrono::high_resolution_clock::now();
#define GT(A, B) std::chrono::duration_cast<chrono::microseconds>(aka##B - aka##A).count()

#endif
/// @brief Print vendor and renderer
void print_spec() {
    const GLubyte *vendor = glGetString(GL_VENDOR);     // Returns the vendor
    const GLubyte *renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    std::cout << vendor << '\t' << renderer << std::endl;
}
namespace GL {

/// @brief Window Creater
class Core {
    /*
    Example
     GL::Core Core("Grapher", { width,height }, GL::Core::InitType::IT_OpenGL_ImGui,
         [](GLFWwindow* wnd) {
             glfwSetKeyCallback(wnd, KeyCallback);
         }
         ,
             [](ImGuiIO* ptr) {
             ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
         }
         );
     */
  private:
  public:
    /// @brief Initialization type {gl, gui, both}
    enum InitType {
        IT_OpenGL = 1,
        IT_ImGui = 2,
        IT_OpenGL_ImGui = IT_OpenGL | IT_ImGui
    };
    std::string mWindowName;

    GLFWwindow *m_window;
    ImGuiIO *imgui_io;

    /// @brief Initialize GLFW window
    /// @param name Window's name
    /// @param width Window's width
    /// @param height Window's Height
    /// @param ConfigGL A pointer to void function with specific configuration during initalization
    virtual void init_window(const char *name, int width, int height, int Profile,
                             void (*ConfigGL)(GLFWwindow *) = NULL) {
        glfwInit();
        m_window = GL::Initialize_GL(name, width, height, Profile);
        window = m_window;
        if (ConfigGL != NULL)
            ConfigGL(m_window);
    }
    /// @brief Initialize ImGui
    /// @param ConfigImGui A pointer to a function to configure ImGuiIO
    virtual void init_imgui(void (*ConfigImGui)(ImGuiIO *) = NULL) {
        const char *glsl_version = "#version 460";
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_io = &ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        imgui_io->Fonts->AddFontFromFileTTF("/home/hussein/Dev/OpenGL/MeshEd/dependencies/calibrib.ttf", 15);

        imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // io.ConfigFlags |= dock;           // Enable Docking
        imgui_io->ConfigFlags |=
            ImGuiViewportFlags_::ImGuiViewportFlags_IsPlatformWindow; // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();
        if (ConfigImGui != NULL)
            ConfigImGui(imgui_io);
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }
    /// @brief Initialize GLFW + ImGui
    /// @param _window_name Window's name
    /// @param size Window's size {Width, Height}
    /// @param Init Which parts to initialize
    /// @param ConfigGL GL additional configuration function
    /// @param ConfigImGui ImGui additional configuration function
    Core(const char *_window_name, const glm::ivec2 &size, InitType Init, void (*ConfigGL)(GLFWwindow *) = NULL,
         void (*ConfigImGui)(ImGuiIO *) = NULL, int OpenGLProfile = GLFW_OPENGL_CORE_PROFILE) {
        this->mWindowName = _window_name;
        if (Init & IT_OpenGL)
            init_window(mWindowName.c_str(), size.x, size.y, OpenGLProfile, ConfigGL);
        if (Init & IT_ImGui)
            init_imgui(ConfigImGui);
    }

    ~Core() {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        glfwTerminate();
    }
};
} // namespace GL

#endif
