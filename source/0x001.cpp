#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <Core2.h>
#include <Program.h>
#include <CoreUtils/CameraControl.h>
#include <CoreUtils/MyPrograms.h>


using namespace std;

int main() {


    glfwInit();
    GLFWwindow* window = GL::Initialize_GL("Mesh", 1000, 1000);

    glm::vec3 vertices[] = {
{1,1,0},
{-1,1,0},
{-1,-1,0},
{1,-1,0},
    };
    unsigned int indices[] = { 0,1,2,2,3,0 };
    GLuint MyProg = GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL);
    //DirectCall DC = DirectCall(vertices, 6, GL_LINE_STRIP, GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL));
    iMesh DC = iMesh(DT_ELEMENT, vertices, 4, indices, 6, GL_TRIANGLES, MyProg);
    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(DC);
    Cam.SetPositions({ 1,1,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DC.Draw();
        glfwSwapBuffers(window);
    }

    return 0;
}