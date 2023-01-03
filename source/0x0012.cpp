#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <Cycloid.h>
#include <sock_util.h>

using namespace std;
int width = 1024, height = 1024;

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

    //Line Function

    Cycloid::LineFunction::LineDescription LD = {

        [](float t)->glm::vec3 {
            //return {cos(t),sin(t),t};
            return {t,
                (t <= 0) ? pow((t + 2), 2) : pow(2 - t, 2)

            ,0};
        }
        , -4, 4, 1000

    };

    Cycloid::LineFunction LineFunc(
        LD, Cycloid::ColorMode::CM_SINGLE, -1, 0, [](float t)->const glm::vec4 {return glm::vec4(1, 1, 1, 1);}
    );

    glm::vec4* FunctionVertices = (glm::vec4*)LineFunc.GetVertices();

    Camera Cam("Proj", "View", NULL);
    Cam.AttachProgram(GG);
    Cam.AttachProgram(LineFunc);
    Cam.SetPositions({ 1,1,1 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(2);
    int counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, ObjectPosition);
            Cam.Update();
        }
        //Update Line Function
        {
            glm::vec3 tmp = glm::vec3(counter / 10.f, counter / 10.f, counter / 10.f);
            LineFunc.AppendValue(&tmp, 1);
            counter++;
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GG.Draw();
        LineFunc.Draw();
        glfwSwapBuffers(GL::window);
    }
    glfwTerminate();
    return 0;

}
