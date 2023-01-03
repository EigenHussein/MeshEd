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

// Mesh LinkContours2(const std::vector<glm::vec3>& outer, const std::vector<glm::vec3>& inner) {
//     Mesh MyMesh;

//     int idx_outer = MyMesh.addRawVertices(outer);
//     int idx_inner = MyMesh.addRawVertices(inner);
//     //cout << "inner index :" << idx_inner << "\nouter index :" << idx_outer << endl;

//     int outer_start_offset = -1;
//     float dist = 1e10;

//     for (int i = 0;i < outer.size();i++) {
//         float dist2 = glm::length(MyMesh.raw_vertices[idx_outer] - MyMesh.raw_vertices[idx_inner]);
//         if (dist2 < dist) {
//             dist = dist2;
//             outer_start_offset = i;
//         }
//     }

//     float indicator =
//         glm::dot(
//             glm::cross(
//                 MyMesh.raw_vertices[idx_outer + outer_start_offset + 2] - MyMesh.raw_vertices[idx_outer + outer_start_offset + 1],
//                 MyMesh.raw_vertices[idx_outer + outer_start_offset + 0] - MyMesh.raw_vertices[idx_outer + outer_start_offset + 1]),
//             glm::cross(
//                 MyMesh.raw_vertices[idx_inner + 2] - MyMesh.raw_vertices[idx_inner + 1],
//                 MyMesh.raw_vertices[idx_inner + 0] - MyMesh.raw_vertices[idx_inner + 1])
//         );
//     const int dir = (indicator > 0) ? 1 : -1;

//     //cout << "Indicator :" << dir << endl;

//     vector<int> outer_links;
//     outer_links.push_back(outer_start_offset);
//     //cout << "outer start offset :" << outer_start_offset << endl;

//     for (int i = 1;i < inner.size();i++) {
//         int next_outer_offset = ((outer.size() + outer_links.back() + dir)) % outer.size(); //buggy
//         //cout << "Next outer offset :" << next_outer_offset << endl;
//         if (
//             glm::length(MyMesh.raw_vertices[idx_inner + i] - MyMesh.raw_vertices[idx_outer + outer_links.back()]) //the distance between Mesh
//             <
//             glm::length(MyMesh.raw_vertices[idx_inner + i] - MyMesh.raw_vertices[idx_outer + next_outer_offset])
//             )
//         {
//             outer_links.push_back(outer_links.back());
//             //cout << "A" << endl;
//         }
//         else {
//             outer_links.push_back(next_outer_offset);
//             //cout << "B" << endl;
//         }
//     }

//     for (int i = 0;i < outer_links.size() - 1;i++) {
//         if (outer_links[i] == outer_links[i + 1]) {
//             MyMesh.DefineTriangle({ idx_inner + i,idx_inner + i + 1,idx_outer + outer_links[i] });
//             //cout << i << '\t' << idx_inner + i << idx_inner + i + 1 << idx_outer + outer_links[i] << endl;
//         }
//         else {
//             MyMesh.DefineTriangle({ idx_inner + i,idx_inner + i + 1,idx_outer + outer_links[i] });
//             int next_outer_offset = ((outer.size() + outer_links[i] + dir)) % outer.size();
//             MyMesh.DefineTriangle({ idx_inner + i + 1,idx_outer + next_outer_offset,idx_outer + outer_links[i] });
//         }
//     }
//     //last
//     return MyMesh;

// }
int main() {
    glfwInit();
    GLFWwindow* window = GL::Initialize_GL("Mesh", 1000, 1000);
    //glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, KeyCallback);

    glEnable(GL_CULL_FACE);
    std::vector<glm::vec3> outer;
    std::vector<glm::vec3>inner;

    for (int i = 0;i < 40;i++) {
        float t = 2 * M_PI * i / float(40);
        outer.push_back({ cos(t),sin(t),0 });
    }

    for (int i = 0;i < 50;i++) {
        float t = 2 * M_PI * i / float(49);
        //inner.push_back({ 0.5 * cos(t),0.5 * sin(t),0 });
        inner.push_back({ 0.5 * cos(t + 0.1) + 0.1 * cos(4 * t),0.5 * sin(t + 0.1) + 0.1 * sin(4 * t),0 });
    }


    t_read(0);
    Mesh M = LinkContours2(outer, inner);
    t_read(1);


    auto kek = M.getRenderable();
    GLuint MyProg = GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL);

    cout << endl;
    cout << kek.nVertices << '\t' << kek.nIndices << endl;
    iMesh DC = iMesh(DT_ELEMENT, kek.vertices, kek.nVertices, (unsigned int*)kek.indices, kek.nIndices, GL_LINE_STRIP, MyProg);
    Camera Cam = Camera("Proj", "View", NULL);
    Cam.AttachProgram(DC);
    Cam.SetPositions({ 0,-0.01,2 }, { 0,0,0 });
    Cam.SetPerspective(M_PI_2);
    Cam.Update(CU_PROJ_VIEW);

    glfwSwapInterval(2);
    int counter = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (new_camera_position) {

            Cam.SetPositions(CameraPosition, { 0,0,0 });
            Cam.Update();
            new_camera_position = false;

        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glUseProgram(DC.ProgramID);
        // glBindVertexArray(DC.SceneData->VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DC.SceneData->EBO);
        // glDrawElements(GL_LINE_STRIP, 3 + 3 * ((counter / 60) % 30), GL_UNSIGNED_INT, 0);
        DC.Draw();
        counter++;
        glfwSwapBuffers(window);
    }



    return 0;
}