
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Meshs.h>
#include <MeshGen.h>
#include <Core2.h>
#include <Program.h>
#include <CoreUtils/CameraControl.h>
#include <CoreUtils/MyPrograms.h>
#include <glm/glm.hpp>
#include <math.h>
#include <chrono>
#include <Numerical.h>

using namespace std;

glm::vec3 cylinder(glm::vec2 uv) {
    return { cos(uv.x),sin(uv.x),uv.y };
}
std::vector<glm::vec3> project(const std::vector<glm::vec3>& contour, const glm::vec3 light_point, glm::vec3(*surface)(glm::vec2)) {
    std::vector<glm::vec3> projected_points;
    Line L = Line({ 0,0,0 }, { 1,1,1 });
    LS_Intersection LS = LS_Intersection(&L, surface);
    glm::vec3 last_uvt = { 0,0,0 };
    for (int i = 0;i < contour.size();i++) {
        L = Line(contour[i], light_point - contour[i]);
        last_uvt = LS.Solve(last_uvt, 0.01, (i == 0) ? 30 : 5);
        glm::vec3 real_proj = surface({ last_uvt.x,last_uvt.y });//or L.point(last_uvt.z)
        projected_points.push_back(real_proj);
        //        cout << points.back() << endl;
    }
    return projected_points;

}
int main2() {

    std::vector<glm::vec3> points;
    //std::vector<glm::vec3> uvt;
    const glm::vec3 cnt = { 0,0,1 };
    Line L = Line({ 0,0,0 }, { 1,1,1 });

    LS_Intersection LS = LS_Intersection(&L, cylinder);
    glm::vec3 last_uvt = { 0,0,0 };
    for (int i = 0;i < 50;i++) {
        float t = 2 * M_PI * i / float(49);
        glm::vec3 tmp = { 2 * cos(t),2 * sin(t),0 };
        L = Line(tmp, cnt - tmp);

        last_uvt = LS.Solve(last_uvt, 0.01, (i == 0) ? 20 : 5);
        glm::vec3 real_proj = cylinder({ last_uvt.x,last_uvt.y });
        points.push_back(real_proj);
        cout << points.back() << endl;
    }



    return 0;
}

int main3() {

    auto L = Line({ 1,1,5 }, { -1,-1,-1 });
    LS_Intersection LS = LS_Intersection(&L, cylinder);

    std::vector<glm::vec3> raw_outer;
    std::vector<glm::vec3> raw_inner;
    std::vector<glm::vec3> real_outer;
    std::vector<glm::vec3> real_inner;

    for (int i = 0;i < 40;i++) {
        float t = 2 * M_PI * i / float(40);
        raw_outer.push_back({ 2 * cos(t) + 3,2 * sin(t) + 3,0 });
    }

    for (int i = 0;i < 50;i++) {
        float t = 2 * M_PI * i / float(49);
        //inner.push_back({ 0.5 * cos(t),0.5 * sin(t),0 });
        raw_inner.push_back({ 1.5 * cos(t + 0.1) + 0.1 * cos(10 * t) + 3 ,1.5 * sin(t + 0.1) + 0.1 * sin(10 * t) + 3,0 });
    }

    real_inner = project(raw_inner, { 0,0,1 }, cylinder);
    real_outer = project(raw_outer, { 0,0,1 }, cylinder);



    t_read(0);
    Mesh M = LinkContours2(real_outer, real_inner);
    t_read(1);


    auto kek = M.getRenderable();


    glfwInit();
    GLFWwindow* window = GL::Initialize_GL("Mesh", 1000, 1000);
    //glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, KeyCallback);

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

// void generate_obj(glm::vec3 * vertices)
int generate_obj(const char* filename, const glm::vec3* Vertices, int nVertices, const unsigned int* Indices, int nIndices);
int main() {

    auto L = Line({ 1,1,5 }, { -1,-1,-1 });
    LS_Intersection LS = LS_Intersection(&L, cylinder);

    std::vector<glm::vec3> raw_outer;
    std::vector<glm::vec3> raw_inner;
    std::vector<glm::vec3> real_outer;
    std::vector<glm::vec3> real_inner;

    for (int i = 0;i < 400;i++) {
        float t = 2 * M_PI * i / float(399);
        raw_outer.push_back({ 2 * cos(t) + 3,2 * sin(t) + 3,0 });
    }

    for (int i = 0;i < 500;i++) {
        float t = 2 * M_PI * i / float(499);
        //inner.push_back({ 0.5 * cos(t),0.5 * sin(t),0 });
        raw_inner.push_back({ 1.5 * cos(t + 0.1) + 0.1 * cos(10 * t) + 3 ,1.5 * sin(t + 0.1) + 0.1 * sin(10 * t) + 3,0 });
    }

    real_inner = project(raw_inner, { 0,0,1 }, cylinder);
    real_outer = project(raw_outer, { 0,0,1 }, cylinder);
    std::vector<glm::vec3> additive;
    additive.insert(additive.end(), real_outer.begin(), real_outer.end());
    additive.insert(additive.end(), real_inner.begin(), real_inner.end());

    Mesh tmp = LinkContours2(raw_outer, raw_inner);
    auto kek = tmp.getRenderable();

    glfwInit();
    GLFWwindow* window = GL::Initialize_GL("Mesh", 1000, 1000);
    //glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, KeyCallback);

    GLuint MyProg = GL::GetProgram("../shaders/0x001/vertex.glsl", "../shaders/0x001/fragment.glsl", NULL);

    //iMesh DC = iMesh(DT_ELEMENT, kek.vertices, kek.nVertices, (unsigned int*)kek.indices, kek.nIndices, GL_LINE_STRIP, MyProg);

    iMesh DC = iMesh(DT_ELEMENT, additive.data(), kek.nVertices, (unsigned int*)kek.indices, kek.nIndices, GL_TRIANGLES, MyProg);



    cout << "Writing file : " << generate_obj("output.obj", additive.data(), kek.nVertices, (unsigned int*)kek.indices, kek.nIndices) << endl;
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
        // glDrawArrays(GL_LINE_STRIP, 0, real_inner.size());
        // glDrawArrays(GL_LINE_STRIP, real_inner.size(), real_outer.size());
        DC.Draw();
        counter++;
        glfwSwapBuffers(window);
    }


    return 0;
}

string vector2string(const glm::vec3& v) {
    char vstr[40];
    sprintf(vstr, "v %6f %6f %6f\n", v.x, v.y, v.z);
    // string tmp;
    // tmp.insert(tmp.end(), vstr, vstr + strlen(vstr));
    return std::string(vstr);//tmp;
}

int generate_obj(const char* filename, const glm::vec3* Vertices, int nVertices, const unsigned int* Indices, int nIndices) {
    char vstr[40];
    std::stringstream sfile;
    sfile << "# Blender v3.2.1 OBJ File: ''\n# www.blender.org\nmtllib untitled.mtl\no Meshy\n";
    for (int i = 0;i < nVertices;i++) {
        sprintf(vstr, "v %6f %6f %6f\n", Vertices[i].x, Vertices[i].y, Vertices[i].z);
        sfile << vstr;
    }
    sfile << "\nusemtl Material\ns off\n";
    const unsigned int* ptr = Indices;
    for (int i = 0;i < nIndices / 3;i++) {
        sprintf(vstr, "f %i %i %i\n", ptr[0]+1, ptr[1]+1, ptr[2]+1);
        sfile << vstr;
        ptr += 3;
    }
    ofstream file = ofstream(filename, ios::app | ios::binary);
    if (file.is_open()) {
        file << sfile.rdbuf();
        file.close();
        return 0;
    }
    return -1;

}


