#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl_init.h>

#include <Cycloid.h>
#include <sock_util.h>

using namespace std;
int width = 1024, height = 1024;

class Wave1D {

private:
    int mNt, mNi;
    glm::vec4* mesh;
    float mx0, mxL, mut0;
    float mC;
    void estimate_U_t1() {
        glm::vec4* U_t1 = mesh + mNi;
        U_t1[0].x = mesh[0].x;
        U_t1[0].y = mx0;//boundary
        U_t1[mNi - 1].x = mesh[mNi - 1].x;
        U_t1[mNi - 1].y = mxL;//boundary
        for (int i = 1;i < mNi - 1;i++) {
            U_t1[i].x = mesh[i].x;
            U_t1[i].y = mesh[i].y - 0.5 * mC * mC *
                (mesh[i + 1].y - 2 * mesh[i].y + mesh[i - 1].y);
        }
    }
    void estimate_U_tk(const int k) {

        glm::vec4* U_tnp1 = mesh + k * mNi;//u(x,t)=u(x,n+1)
        glm::vec4* U_tn = mesh + (k - 1) * mNi;//u(x,t)=u(k,n)
        glm::vec4* U_tnm1 = mesh + (k - 2) * mNi;//u(x,t)=u(k,n-1)


        U_tnp1[0].x = mesh[0].x;
        U_tnp1[0].y = mx0;//boundary
        U_tnp1[mNi - 1].x = mesh[mNi - 1].x;
        U_tnp1[mNi - 1].y = mxL;//boundary

        for (int i = 1;i < mNi - 1;i++) {
            U_tnp1[i].x = U_tn[i].x;
            U_tnp1[i].y = -U_tnm1[i].y + 2 * U_tn[i].y + mC * mC *
                (U_tn[i + 1].y - 2 * U_tn[i].y + U_tn[i - 1].y);
        }
    }
public:
    Wave1D() {}
    Wave1D(int Nt, int Ni) :mNt(Nt), mNi(Ni) {
        mesh = new glm::vec4[Nt * Ni];
    }

    void SetInit(glm::vec4* initial_condition, float x0 = 0, float xL = 0, float ut0 = 0) {
        memcpy(mesh, initial_condition, sizeof(glm::vec4) * mNi);
        mx0 = x0;
        mxL = xL;
        mut0 = ut0;
        mC = 1;
    }
    void SolveMesh(const float C) {
        this->mC = C;
        estimate_U_t1();
        for (int i = 2;i < mNt - 2;i++) {
            estimate_U_tk(i);
        }
    }
    glm::vec4* getMesh() const {
        return mesh;
    }
    ~Wave1D() {
        delete[] mesh;
    }


};


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

    int cnt;

    Wave1D WV(1000, 1000);
    WV.SetInit(FunctionVertices);
    WV.SolveMesh(1);

    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        if (new_camera_position) {
            Cam.SetPositions(CameraPosition, ObjectPosition);
            Cam.Update();
        }
        //Update Line Function
        {
            glm::vec4* current_tm = WV.getMesh() + ((cnt++) % 1000) * 1000;
            glm::vec4* tmp = (glm::vec4*)glMapNamedBuffer(LineFunc.SceneData->GBuf.buf_idx, GL_READ_WRITE);
            memcpy(tmp, current_tm, sizeof(glm::vec4) * LD.steps);
            glUnmapNamedBuffer(LineFunc.SceneData->GBuf.buf_idx);


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
