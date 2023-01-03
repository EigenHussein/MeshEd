#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

// #include <VirtualScreen.h>
// #include <Fields/ComputeShader.h>
// #include <MeshGenerator.h>
// #include <dynamic_compute.h>

#include <CycloidGraphs.h>
using namespace std;

const int width = 1000, height = 1000;


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

    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

    cout << vendor << '\t' << renderer << endl;

    Cycloid::Grapher2D G2D(10, -1, 1000, 1000);
    glLineWidth(1.5);
    G2D.AddGrid({ 10,10 }, { 5,5 }, { 0.2,0.2 }, { 1,1,1,0.7 }, { 1,1,1,0.3 });
    // G2D.AddGrid({ 10,10 }, { 1,1 }, {}, { 1,0,0 }, { 0,0,1 });
    // G2D.AddGrid({ 3,3 }, { 1,1 }, {}, { 1,0,0 }, { 0,0,1 });
    // G2D.AddGrid({ 3,3 }, { 1,1 }, {}, { 0,1,0 }, { 0,0,1 });
    G2D.Add2DFunction([](float t)->float {return (cos(2 * M_PI * t) / 5.f);}, 0, 2 * M_PI, 100);
    G2D.Finish();

    Camera Cam("Proj", "View", NULL, PT_ORTHOGONAL);
    Cam.AttachProgram(G2D);
    Cam.SetPositions({ 0,0,1 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);

    //Cam.SetOrtho(-10, 10, -10, 10);
    Cam.Update(CU_PROJ_MODEL);


    glfwSwapInterval(2);
    unsigned int arr[5] = { 0,0,0,0,0 };
    int counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        counter++;
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //G2D.Draw();
        T(1);
        glUseProgram(G2D.ProgramID);
        T(2);
        glBindVertexArray(G2D.SceneData->VAO);
        T(3);
        glBindBuffer(GL_ARRAY_BUFFER, G2D.SceneData->VBO);
        T(4);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G2D.SceneData->EBO);
        T(5);
        glDrawElements(GL_LINES, 500, GL_UNSIGNED_INT, 0);
        T(6);
        arr[0] += GT(1, 2);
        arr[1] += GT(2, 3);
        arr[2] += GT(3, 4);
        arr[3] += GT(4, 5);
        arr[4] += GT(5, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(GL::window);
    }

    glfwTerminate();

    for (int i = 0;i < 5;i++) {
        cout << arr[i] / float(counter) << endl;
    }
    return 0;
}
