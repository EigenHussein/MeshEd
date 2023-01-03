#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <Cycloid.h>
#include <sock_util.h>

using namespace std;

const int width = 1000, height = 1000;

void print_spec() {
    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    cout << vendor << '\t' << renderer << endl;
}

int main() {
    printf("Starting sht\n");
    TCPServer TCPserv(1610, 5);
    printf("Waiting for love !\n");
    TCPserv.Accept();
    printf("found !");
    // time_packet tp[100];
    // for (int i = 0;i < 100;i++) {
    //     TCPserv.RecvAll(1, tp + i, sizeof(time_packet), sizeof(time_packet), MSG_WAITALL);
    //     cout << i << '\t' << tp[i].t << endl;
    // }



//    return 0;

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
    Cycloid::Grid GG(5, 5, 3, 3);
    GG.setColor({ 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,1 }, { 0.3 / 0.7,0.6 / 0.7,0.7 / 0.7,0.5 });
    GG.setWidth(1.5, 1);
    GG.Finish();
    
    Cycloid::LineFunction Func(
        Cycloid::LineFunction::LineDescription(
            [](float t)->glm::vec3 {return { cos(t),sin(t),0 };}, 0, 2 * M_PI, 1000
        ), Cycloid::LineFunction::CM_PERVERTEX, -1,
        NULL, [](float t)->const glm::vec4 {
            return { 1,1,1,1 };
            //return (1.f / float(2 * M_PI)) * glm::vec4(t, t, t, t);
        }
    );

    Cycloid::FunctionSweep sweep1(
        &Func, {
            [](float t) {
                return glm::vec3(cos(t),sin(t),-0.001);
            },0,2 * M_PI,1000
        },
        {
            [](float t) {
                return glm::vec3((1 + 0.5 * cos(5 * t)) * cos(t),(1 + 0.5 * cos(5 * t)) * sin(t),-0.001);
            },0,2 * M_PI,1000
        }
        );
    Camera Cam("Proj", "View", NULL);

    Cam.AttachProgram(Func);
    Cam.AttachProgram(GG);
    Cam.SetPositions({ 1,1,1 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_MODEL);

    int counter = 0;
    int hehe = 0;
    time_packet tp;
    glm::vec3 zero_ = { 0.f,0.f,0.f };
    glNamedBufferSubData(Func.SceneData->GBuf.buf_idx, 0, sizeof(zero_), glm::value_ptr(zero_));
    while (!glfwWindowShouldClose(GL::window)) {
        counter++;
        if (counter < 1000000 + 1) {
            TCPserv.RecvAll(1, &tp, sizeof(time_packet), sizeof(time_packet), MSG_WAITALL);
            // cout << tp.val.v[0] << '\t' << tp.val.v[1] << '\t' << tp.val.v[2] << endl;
        }
        glm::vec3 tmp = { tp.val.v[0],tp.val.v[1],tp.val.v[2] };

        //cout << tmp.x << '\t' << tmp.y << '\t' << tmp.z << endl;

        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, {});
            Cam.Update();
        }
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //T(1);
        float t = glfwGetTime();
        zero_ = { 10.f * tp.val.v[0],10.f * tp.val.v[1],10.f * tp.val.v[2] };
        glNamedBufferSubData(Func.SceneData->GBuf.buf_idx, sizeof(Cycloid::ColoredVertex), sizeof(zero_), glm::value_ptr(zero_));
        //Func.AppendValue(&tmp, 1);
        // sweep1.lerp(1);
        GG.Draw();
        //Func.Draw2(3);
        glUseProgram(Func.ProgramID);
        glBindVertexArray(Func.SceneData->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, Func.SceneData->GBuf.buf_idx);
        glDrawArrays(GL_LINES, 0, 2);
     //   T(2);
        //hehe += GT(1, 2);
        glfwSwapBuffers(GL::window);
    }
    cout << hehe / float(counter) << endl;

    glfwTerminate();

    return 0;
}

