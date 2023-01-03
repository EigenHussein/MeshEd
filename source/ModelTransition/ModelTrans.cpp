#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Cycloid.h>

#define _USE_TIMERS
#include <complex>
#include <gl_init.h>
// Latex
#include <EqTexGL.h>
#include <Model.h>
#include <SVGParser.h>
#include <memory.h>
#include <memory>

using namespace std;

// struct VertexInfo {
//     glm::vec4 position;
//     glm::vec3 normal;
//     glm::vec3 color;
//     glm::vec2 uv;
//     VertexInfo() : position({}), normal({}), color({}), uv({}) {
//     }
// };

struct VertexInfo {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 color;
    // glm::vec2 uv;
    VertexInfo() : position({}), normal({}), color({}) /* , uv({}) */ {
    }
};

struct MeshInfo {
  private:
    bool initialized;

  public:
    VertexInfo *data;
    unsigned int count;
    unsigned int number_of_triangles;
    MeshInfo(unsigned int count)
        : initialized(true), data(new VertexInfo[count]), count(count), number_of_triangles(count / 3) {
    }
    void Free() {
        if (initialized && count > 0)
            delete[] data;
    }
};

MeshInfo getRawMesh(Model &model) {
    auto vertices = model.GetVertices();
    auto normals = model.GetNormals();
    auto uvs = model.GetUVs();
    auto indices = model.GetIndices();
    auto nIndices = model.nIndices();

    MeshInfo myMesh(nIndices);
    printf("number of indices: %i\t estimated size is %i bytes\n", nIndices, sizeof(VertexInfo) * nIndices);
    for (int i = 0; i < nIndices; i++) {
        auto &tmp = myMesh.data[i];
        const glm::vec2 &current_uv = uvs[indices[i]];
        tmp.position = {vertices[indices[i]], 0};
        tmp.normal = {normals[indices[i]], current_uv.x};
        tmp.color = {1, 1, 1, current_uv.x};
        // tmp.uv = uvs[indices[i]];
    }

    return myMesh;
}

ArrayBuffer *UploadRawMesh(MeshInfo &model) {
    ArrayBuffer *AB = new ArrayBuffer(DT_VERTEX);
    AB->AllocateBuffer(
        AttributedData({DataBuffer(model.data, model.count * sizeof(VertexInfo))}, {GLAttrib(0, 4, 12, 0), // Position
                                                                                    GLAttrib(1, 3, 12, 4), // Normal
                                                                                    GLAttrib(2, 3, 12, 7), // Color
                                                                                    GLAttrib(3, 2, 12, 10)}));
    return AB;
}

struct Triangle {
    VertexInfo tri[3];
    Triangle() : tri({{}, {}, {}}) {
    }
};

const glm::vec4 direction = glm::vec4(0, 0, 1, 0) / sqrt(3.f);
bool farther(const Triangle &a, const Triangle &b) {
    glm::vec4 a_avg = (a.tri[0].position + a.tri[1].position + a.tri[2].position) / 3.f;
    glm::vec4 b_avg = (b.tri[0].position + b.tri[1].position + b.tri[2].position) / 3.f;
    return glm::dot(a_avg, direction) > glm::dot(b_avg, direction);
}

int main() {

    GL::Core mCore = GL_CORE_DEFAULT(1000, 1000);

    Model mModel = Model("/home/hussein/Desktop/General/3Ds/2022.obj");
    MeshInfo myMesh = getRawMesh(mModel);

    // Sort triangles;

    Triangle *tri_ptr = (Triangle *)myMesh.data;
    auto a = chrono::high_resolution_clock::now();
    sort(tri_ptr, tri_ptr + myMesh.number_of_triangles, farther);
    auto b = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::microseconds>(b - a).count() << endl;
    ArrayBuffer *AB = UploadRawMesh(myMesh);
    // ModelExplorer Mex = ModelExplorer(mModel);
    ProgramContainer PC = ProgramContainer(
        GL::GetProgram("../shaders/ModelTransition/vertex.glsl", "../shaders/ModelTransition/fragment.glsl", NULL));
    PC.direct_attach(*AB);

    ComputeShader MeshModifier("../shaders/ModelTransition/compute.glsl");
    glUseProgram(MeshModifier.ProgramID);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, AB->GBuf.buf_idx);
    glDispatchCompute(myMesh.number_of_triangles, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    Camera mCam = Camera("Proj", "View", NULL);
    {
        mCam.SetPositions({0, 0, 0}, {1, 1, 1});
        mCam.SetPerspective(M_PI_2);
        mCam.AttachProgram(PC);
        mCam.Update(CU_PROJ_VIEW);
    }
    glfwSwapInterval(1);
    int counter = 0;
    while (!glfwWindowShouldClose(GL::window)) {
        glfwPollEvents();
        UpdateOnChange(mCam);
        glUseProgram(MeshModifier.ProgramID);
        glDispatchCompute((counter <= myMesh.number_of_triangles) ? counter : myMesh.number_of_triangles, 1, 1);
        counter +=10;
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GUI::StartImGui();
        static int count = myMesh.count;
        ImGui::SliderInt("number of vertices", &count, 0, myMesh.count);
        PC.Draw(GL_TRIANGLES, 0, count);
        GUI::RenderImGui();
        glfwSwapBuffers(GL::window);
    }

    glfwTerminate();
    return 0;
}