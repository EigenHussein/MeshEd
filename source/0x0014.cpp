
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>
//#define _USE_TIMERS
#include <gl_init.h>

#include <opencv2/opencv.hpp>
#include <string_proc.h>

#include <svg_wrapping.h>

using namespace std;

rapidxml::xml_document<> doc;
const int width = 1000, height = 1000;

#include <EqTexGL.h>

int main() {

    GL::Core Core = GL::Core(
        "Grapher", {width, height}, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },
        [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }, GLFW_OPENGL_COMPAT_PROFILE);
    glDisable(GL_DEPTH_TEST);
    print_spec();

    auto svg_path = Cycloid::EquationGenerator::GenerateSVG();
    auto eq_render = Cycloid::EquationRenderer(doc, svg_path);

    eq_render.InitGraphics();
    // eq_render.SetMatrix(CU_PROJECTION, glm::ortho(-1.f, 1.f, -1.f, 1.f));
    // eq_render.SetMatrix(CU_VIEW, glm::identity<glm::mat4>());
    eq_render.Update(CU_PROJ_VIEW);

    GLuint glyphBase = glGenPathsNV(6);

    const char *word = "OpenGL";
    const GLsizei wordLen = (GLsizei)strlen(word);
    const GLfloat emScale = 2048;   // match TrueType convention
    GLuint templatePathObject = ~0; // Non-existant path object
    glPathGlyphsNV(glyphBase, GL_SYSTEM_FONT_NAME_NV, "Helvetica", GL_BOLD_BIT_NV, wordLen, GL_UNSIGNED_BYTE, word,
                   GL_SKIP_MISSING_GLYPH_NV, templatePathObject, emScale);

    glfwSwapInterval(2);
    int counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();

        // glClearStencil(0);
        // glStencilMask(~0);
        // glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // glStencilFillPathNV(glyphBase, GL_COUNT_UP_NV, 0x1F);
        // glEnable(GL_STENCIL_TEST);
        // glColor3f(1, 1, 1); // white
        // glCoverFillPathNV(glyphBase, GL_BOUNDING_BOX_NV);
        // glPathParameteriNV(glyphBase, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);

        // glDisable(GL_STENCIL_TEST);

        eq_render.Draw(counter++);

        glfwSwapBuffers(GL::window);
    }
    return 0;
}