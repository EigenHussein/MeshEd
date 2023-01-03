#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <MeshGen.h>
#include <Core2.h>
#include <Program.h>
#include <CoreUtils/CameraControl.h>
#include <CoreUtils/MyPrograms.h>

#include <chrono>

using namespace std;
ostream& operator<<(ostream& o, const glm::vec3& p) {
    o << "[" << p.x << ',' << p.y << ',' << p.z << "]";
    return o;
}

ostream& operator<<(ostream& o, const std::vector<int>& v) {
    for (auto i : v) {
        o << i << '\t';
    }
    return o;
}

void process(std::vector<std::vector<int>>& v) {
    for (int i = 0;i < 1000;i++) {
        int cind = rand() % 10000;
        v[cind].erase(v[cind].begin());
        cind = rand() % 10000;
        v.erase(v.begin() + cind);
    }
}

int main() {


    glfwInit();
    GLFWwindow* window = GL::Initialize_GL("Mesh", 1000, 1000);
    //glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, KeyCallback);
    auto func = [](glm::vec2 t) {
        return glm::vec3(t.x, t.y, exp(-5 * t.x * t.x - 5 * t.y * t.y));
    };
    t_read(0);
    Mesh M = GetMesh(
        func
        , { 1,1 }, { -1,-1 }, { 100,100 });
    t_read(1);
    // for (int i = 0;i < 2000;i++) {
    //     int r = rand() % 20000;
    //     if (M.Triangles[r].ID != -1) {
    //         M.removeTriangle(r);
    //     }
    // }
    int top_tri_id = M.detachTriangle(2 * 100 * 50 + 100);
    M.raw_vertices[M.Triangles[top_tri_id].verticesIDs[0]].z += 0.1;
    M.raw_vertices[M.Triangles[top_tri_id].verticesIDs[1]].z += 0.1;
    M.raw_vertices[M.Triangles[top_tri_id].verticesIDs[2]].z += 0.1;
    
    for (int i = 0;i < 100;i++) {
        cout << M.AreNeighbours(0, i) << endl;
    }

    auto kek = M.getRenderable();
    GLuint MyProg = GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL);

    cout << endl;
    cout << kek.nVertices << '\t' << kek.nIndices << endl;
    iMesh DC = iMesh(DT_ELEMENT, kek.vertices, kek.nVertices, (unsigned int*)kek.indices, kek.nIndices, GL_TRIANGLES, MyProg);
    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(DC);
    Cam.SetPositions({ 1,1,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(2);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (new_camera_position) {

            Cam.SetPositions(CameraPosition, { 0,0,0 });
            Cam.Update();
            new_camera_position = false;

        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DC.Draw();
        glfwSwapBuffers(window);
    }

    return 0;
}