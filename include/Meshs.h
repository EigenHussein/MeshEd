#ifndef MESHS_HH
#define MESHS_HH

#include <glm/glm.hpp>

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <memory.h>

std::ostream& operator<<(std::ostream& o, const glm::vec3& p) {
    o << "[" << p.x << ',' << p.y << ',' << p.z << "]";
    return o;
}


template<typename _T> void DeleteElement(std::vector<_T>& vec, const _T& elem) {
    for (int i = 0;i < vec.size();i++) {
        if (vec[i] == elem) {
            vec.erase(vec.begin() + i);
            return;
        }
    }
}

struct StructuredMesh {
private:
    bool initialized;
public:
    glm::vec3* vertices;
    size_t nVertices;
    int* indices;
    size_t nIndices;
    StructuredMesh(glm::vec3* V, uint nV, int* I, uint nI) :vertices(V), nVertices(nV), indices(I), nIndices(nI), initialized(((nV + nI) > 0) ? true : false) {}
    void Free() {
        if (initialized) {
            free(vertices);
            free(indices);
            vertices = 0;
            indices = 0;
            initialized = false;
        }
    }
};


class Triangle;
//class Edge;
class Vertex;
class Mesh;

class Triangle {

public:

    int ID;
    int verticesIDs[3];
    //int edgesIDs[3];
    std::vector<int> neighbour_triangles;

    Mesh* mymesh;
    Triangle(const std::vector<glm::vec3>& vertices) {

    }

    Triangle(Mesh* _mesh) {
        mymesh = _mesh;
    }
};

class Vertex {
public:
    int ID;
    std::vector<int> trianglesIDs;
    Mesh* mymesh;
    Vertex(Mesh* _mesh, int id) {
        ID = id;
        mymesh = _mesh;
    }

};
// class Edge {
// public:
//     int ID;
//     std::vector<int> trianglesIDs;
//     Mesh* mymesh;
//     Vertex(Mesh* _mesh, int id) {
//         ID = id;
//         mymesh = _mesh;
//     }

// };

class Mesh {
private:
    // template<typename _T> _T* Copy(const std::vector<_T>& arr) {
    //     uint buf_size = sizeof(_T) * arr.size();
    //     _T* tmp = (_T*)malloc();
    //     memcpy(tmp, arr.data(), buf_size);
    //     return tmp;

    // }
public:
    bool AreNeighbours(int Triangle1, int Triangle2) {
        Triangle& T1 = Triangles[Triangle1];
        Triangle& T2 = Triangles[Triangle2];
        for (int i = 0;i < 3;i++) {
            for (int j = 0;j < 3;j++) {
                if (T1.verticesIDs[i] == T2.verticesIDs[j]) {
                    if (
                        T1.verticesIDs[(i + 1) % 3] == T2.verticesIDs[(j + 3 - 1) % 3] ||
                        T2.verticesIDs[(j + 1) % 3] == T1.verticesIDs[(i + 3 - 1) % 3]
                        ) {
                        return true;
                    }
                    else {
                        return false;
                    }
                }
            }
        }

        return false;
    }
    uint currentID; int index_counter;
public:

    //mesh raw information
    std::vector<glm::vec3> raw_vertices;


    //relations information
    std::vector<Triangle> Triangles;
    //std::vector<int> ActiveTriangles;
    std::vector<Vertex> Vertices;
    //std::vector<int> ActiveVertices;

    Mesh() {
        currentID = 0;
        index_counter = 0;
    }
    inline int emit_index() {
        return index_counter++;
    }
    /*

        add three vertices
        add three indices
        add one triangle

    */
    uint NewTriangle(const std::vector<glm::vec3>& verts) {
        assert(verts.size() == 3);
        Triangles.push_back(Triangle(this));
        Triangle& currentTriangle = Triangles.back();
        currentTriangle.ID = Triangles.size() - 1;// index of triangle , Ex. 0 for the first triangle

        for (int i = 0;i < 3;i++) {
            //number of current indices , which is the same as next vertex's index
            int current_index = emit_index();
            //append the new real vertex
            raw_vertices.push_back(verts[i]);
            //relation vertex structure
            Vertices.push_back(Vertex(this, raw_vertices.size() - 1));
            //link the triangle to every vertex
            Vertices.back().trianglesIDs.push_back(currentTriangle.ID);
            //link the vertices to the triangle
            currentTriangle.verticesIDs[i] = current_index;

        }
        return currentTriangle.ID;

    }
    /*
        add triangle using 3 indices

    */
    uint DefineTriangle(const std::vector<int>& new_inds) {
        //std::cout << "TRI" << std::endl;
        //check validity
        assert(new_inds.size() == 3);
        //add blank triangle
        Triangles.push_back(Triangle(this));
        Triangle& currentTriangle = Triangles.back();
        //triangle's ID is vector size - 1
        currentTriangle.ID = Triangles.size() - 1;

        for (int i = 0;i < 3;i++) {
            //set triangle's vertices
            currentTriangle.verticesIDs[i] = new_inds[i];
            //link every vertex to the triangle
            Vertices[new_inds[i]].trianglesIDs.push_back(currentTriangle.ID);

        }

        return currentTriangle.ID;

    }
    uint addRawVertices(const std::vector<glm::vec3>& verts) {
        int ret = index_counter;
        for (int i = 0;i < verts.size();i++) {
            raw_vertices.push_back(verts[i]);
            int currentIndex = emit_index();
            Vertices.push_back(Vertex(this, currentIndex));
        }
        return ret;
    }
    uint addSubMesh(const glm::vec3* verts, size_t nV, const uint* inds, size_t nI, int index_ref = -1) {
        index_ref = emit_index();
        for (int i = 0;i < nV;i++) {
            int newIndex = (i == 0) ? index_ref : emit_index();
            raw_vertices.push_back(verts[i]);
            Vertices.push_back(Vertex(this, newIndex));
        }

        for (int i = 0;i < nI / 3;i++) {
            DefineTriangle({ index_ref + 3 * i + 0,index_ref + 3 * i + 1,index_ref + 3 * i + 2 });
        }
        return nI;
    }
    int pushVertex(const glm::vec3& v) {
        raw_vertices.push_back(v);
        int currentID = emit_index();
        Vertices.push_back(Vertex(this, currentID));
        return currentID;
    }
    StructuredMesh getRenderable() {
        uint buf_size = sizeof(glm::vec3) * raw_vertices.size();
        glm::vec3* vtmp = (glm::vec3*)malloc(buf_size);
        memcpy(vtmp, raw_vertices.data(), buf_size);

        std::vector<uint> indices/* (raw_vertices.size()) */;

        for (auto a : Triangles) {
            if (a.ID != -1) {
                indices.push_back(a.verticesIDs[0]);
                indices.push_back(a.verticesIDs[1]);
                indices.push_back(a.verticesIDs[2]);
            }
        }

        buf_size = sizeof(int) * indices.size();
        int* itmp = (int*)malloc(buf_size);
        memcpy(itmp, indices.data(), buf_size);
        return { vtmp,raw_vertices.size(),itmp,indices.size() };
    }

    void CenterPoint(uint trianlgeID) {
        uint inds[3] = {
            this->Triangles[trianlgeID].verticesIDs[0],
            this->Triangles[trianlgeID].verticesIDs[1],
            this->Triangles[trianlgeID].verticesIDs[2] };

        glm::vec3 center = raw_vertices[inds[0]] +
            (raw_vertices[inds[1]] - raw_vertices[inds[0]]) * 0.5f +
            (raw_vertices[inds[2]] - raw_vertices[inds[1]]) * 0.5f;
        NewTriangle({ raw_vertices[inds[0]],raw_vertices[inds[1]],center });
        NewTriangle({ raw_vertices[inds[1]],raw_vertices[inds[2]],center });
        NewTriangle({ raw_vertices[inds[2]],raw_vertices[inds[0]],center });
    }

    void divideInto3(uint trianlgeID) {
        int inds[3] = {
           this->Triangles[trianlgeID].verticesIDs[0],
           this->Triangles[trianlgeID].verticesIDs[1],
           this->Triangles[trianlgeID].verticesIDs[2] };

        glm::vec3 center = raw_vertices[inds[0]] +
            (raw_vertices[inds[1]] - raw_vertices[inds[0]]) * 0.5f +
            (raw_vertices[inds[2]] - raw_vertices[inds[1]]) * 0.25f;
        int center_index = pushVertex(center);
        // uint new_inds[9] = {
        //     inds[0],inds[1],center_index,
        //     inds[1],inds[2],center_index,
        //     inds[2],inds[0],center_index
        // };
        int new_triangles_ids[3];
        new_triangles_ids[0] = DefineTriangle({ inds[0],inds[1],center_index });
        //std::cout << "P1" << std::endl;
        new_triangles_ids[1] = DefineTriangle({ inds[1],inds[2],center_index });
        //std::cout << "P2" << std::endl;
        new_triangles_ids[2] = DefineTriangle({ inds[2],inds[0],center_index });

        // int tmp = 3;
        // for (int i = 0;i < 3;i++) {
        //     for (int j = 0;j < 3;j++) {
        //         if (i != j) {
        //             Triangles[new_triangles_ids[i]].neighbour_triangles.push_back(new_triangles_ids[((tmp++) / 2) % 3]);
        //         }
        //     }
        // }

        Triangles[new_triangles_ids[0]].neighbour_triangles.push_back(new_triangles_ids[1]);
        Triangles[new_triangles_ids[0]].neighbour_triangles.push_back(new_triangles_ids[2]);
        Triangles[new_triangles_ids[1]].neighbour_triangles.push_back(new_triangles_ids[2]);
        Triangles[new_triangles_ids[1]].neighbour_triangles.push_back(new_triangles_ids[0]);
        Triangles[new_triangles_ids[2]].neighbour_triangles.push_back(new_triangles_ids[0]);
        Triangles[new_triangles_ids[2]].neighbour_triangles.push_back(new_triangles_ids[1]);

        //std::cout << "P3" << std::endl;
    }
    //apperantly remove a triangle
    int removeTriangle(int TriangleID) {
        Triangle& target = Triangles[TriangleID];

        for (auto i : target.neighbour_triangles) {
            Triangle& neighbour = Triangles[i];
            DeleteElement(neighbour.neighbour_triangles, target.ID);
        }
        for (auto i : target.verticesIDs) {
            Vertex& vert = Vertices[i];
            DeleteElement(vert.trianglesIDs, target.ID);
        }
        target.ID = -1;

        return 0;
    }
    int detachTriangle(int TriangleID) {
        Triangle& currentTriangle = Triangles[TriangleID];
        for (int i = 0;i < 3;i++) {
            glm::vec3& old_vertex = raw_vertices[currentTriangle.verticesIDs[i]];
            Vertex& old_vertex_ref = Vertices[currentTriangle.verticesIDs[i]];
            int new_vertex_ref = pushVertex(old_vertex);
            DeleteElement(old_vertex_ref.trianglesIDs, TriangleID);
            currentTriangle.verticesIDs[i] = new_vertex_ref;
        }
        return TriangleID;
    }
};

Mesh LinkContours2(const std::vector<glm::vec3>& outer, const std::vector<glm::vec3>& inner) {
    Mesh MyMesh;

    int idx_outer = MyMesh.addRawVertices(outer);
    int idx_inner = MyMesh.addRawVertices(inner);
    //cout << "inner index :" << idx_inner << "\nouter index :" << idx_outer << endl;

    int outer_start_offset = -1;
    float dist = 1e10;

    for (int i = 0;i < outer.size();i++) {
        float dist2 = glm::length(MyMesh.raw_vertices[idx_outer] - MyMesh.raw_vertices[idx_inner]);
        if (dist2 < dist) {
            dist = dist2;
            outer_start_offset = i;
        }
    }

    float indicator =
        glm::dot(
            glm::cross(
                MyMesh.raw_vertices[idx_outer + outer_start_offset + 2] - MyMesh.raw_vertices[idx_outer + outer_start_offset + 1],
                MyMesh.raw_vertices[idx_outer + outer_start_offset + 0] - MyMesh.raw_vertices[idx_outer + outer_start_offset + 1]),
            glm::cross(
                MyMesh.raw_vertices[idx_inner + 2] - MyMesh.raw_vertices[idx_inner + 1],
                MyMesh.raw_vertices[idx_inner + 0] - MyMesh.raw_vertices[idx_inner + 1])
        );
    const int dir = (indicator > 0) ? 1 : -1;

    //cout << "Indicator :" << dir << endl;

    std::vector<int> outer_links;
    outer_links.push_back(outer_start_offset);
    //cout << "outer start offset :" << outer_start_offset << endl;

    for (int i = 1;i < inner.size();i++) {
        int next_outer_offset = ((outer.size() + outer_links.back() + dir)) % outer.size(); //buggy
        //cout << "Next outer offset :" << next_outer_offset << endl;
        if (
            glm::length(MyMesh.raw_vertices[idx_inner + i] - MyMesh.raw_vertices[idx_outer + outer_links.back()]) //the distance between Mesh
            <
            glm::length(MyMesh.raw_vertices[idx_inner + i] - MyMesh.raw_vertices[idx_outer + next_outer_offset])
            )
        {
            outer_links.push_back(outer_links.back());
            //cout << "A" << endl;
        }
        else {
            outer_links.push_back(next_outer_offset);
            //cout << "B" << endl;
        }
    }

    for (int i = 0;i < outer_links.size() - 1;i++) {
        if (outer_links[i] == outer_links[i + 1]) {
            MyMesh.DefineTriangle({ idx_inner + i,idx_inner + i + 1,idx_outer + outer_links[i] });
            //cout << i << '\t' << idx_inner + i << idx_inner + i + 1 << idx_outer + outer_links[i] << endl;
        }
        else {
            MyMesh.DefineTriangle({ idx_inner + i,idx_inner + i + 1,idx_outer + outer_links[i] });
            int next_outer_offset = ((outer.size() + outer_links[i] + dir)) % outer.size();
            MyMesh.DefineTriangle({ idx_inner + i + 1,idx_outer + next_outer_offset,idx_outer + outer_links[i] });
        }
    }
    //last
    return MyMesh;

}

#endif