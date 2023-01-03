#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>
//#define _USE_TIMERS
#include <gl_init.h>

using namespace std;
const int width = 1024, height = 1024;
void GetCameraMat(glm::vec3 Opos, glm::vec3 Cpos, glm::vec3 vUP = glm::vec3(0, 0, 1)) {
    glm::vec3 CamDir = Opos - Cpos;
    glm::vec3 Right = glm::cross(CamDir, vUP);
    glm::vec3 Up = glm::cross(Right, CamDir);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(Cpos.x, Cpos.y, Cpos.z, Opos.x, Opos.y, Opos.z, Up.x, Up.y, Up.z);
}
int main() {
    GL::Core Core = GL::Core(
        "Grapher", {width, height}, GL::Core::InitType::IT_OpenGL_ImGui,
        [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },
        [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }, GLFW_OPENGL_COMPAT_PROFILE);

    glDisable(GL_DEPTH_TEST);
    print_spec();
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    GetCameraMat({0, 0, 0}, {1, 1, 1});
    glMatrixMode(GL_PROJECTION);
    // glOrtho(-2, 2, -2, 2, -10, 10);
    auto k = glm::perspective((float)M_PI_2, 1.f, 0.01f, 100.f);
    glLoadMatrixf(glm::value_ptr(k));
    // gluPerspective(120, 1, 0.01, 100);

    //  // glPushMatrix()
    //  // glLoadIdentity()
    //  GLfloat light0_position[] = {0, 0, 1, 0};
    //  GLfloat light0_brightness[] = {1, 1, 1, 1};
    //  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    //  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_brightness);
    //  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_brightness);

    // GLfloat material_diffuse[] = {1, 0, 0, 1};
    // GLfloat material_specular[] = {1, 1, 1, 1};
    // GLfloat material_shininess[] = {100};
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    // glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glShadeModel(GL_SMOOTH);
        glBegin(GL_TRIANGLES);

        glColor3f(0.1, 0.2, 0.3);
        glVertex3f(0, 0, 0);
        glColor3f(0.4, 0.5, 0.6);
        glVertex3f(1, 0, 0);
        glColor3f(0.7, 0.8, 0.9);
        glVertex3f(0, 1, 0);

        glColor3f(0.1, 0.2, 0.3);
        glVertex3f(1, 1, 0);
        glColor3f(0.4, 0.5, 0.6);
        glVertex3f(1, 0, 0);
        glColor3f(0.7, 0.8, 0.9);
        glVertex3f(0, 1, 0);
        glEnd();

        glfwSwapBuffers(GL::window);
    }

    return 0;
}