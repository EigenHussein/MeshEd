#ifndef SHAPE_UTILITY_HH
#define SHAPE_UTILITY_HH

#include <Core2.h>
#include <Fields/ComputeShader.h>
#include <Program.h>
#include <glm/glm.hpp>
#include <initializer_list>
#include <iostream>
#include <vector>
#define ASSIGN_XYZ(A, B)                                                                                               \
    (A).x = (B).x;                                                                                                     \
    (A).y = (B).y;                                                                                                     \
    (A).z = (B).z;                                                                                                     \
    (A).a = 0.f
namespace ShapeGenerators {

/// @brief Append Generate Quad
/// @tparam _VERTEX_TYPE vec3,vec4, ColoredVertex...
/// @param prim Quad primitives
/// @param vertices Pointer to the pointer of vertices
/// @param indices
/// @param vertex_size
template <typename _VERTEX_TYPE>
void Quad(const std::initializer_list<glm::vec3> prim, _VERTEX_TYPE **vertices, unsigned int new_id = 0,
          unsigned int **indices = NULL, const int vertex_size = sizeof(glm::vec4)) {
    // (**vertices).x=prim[0].x;
    // (**vertices).y=prim[0].y;
    // ASSIGN_XYZ((**vertices), *(prim.begin() + 0));
    // (*vertices)++;
    // ASSIGN_XYZ((**vertices), *(prim.begin() + 1));
    // (*vertices)++;
    // ASSIGN_XYZ((**vertices), *(prim.begin() + 2));
    // (*vertices)++;
    // ASSIGN_XYZ((**vertices), *(prim.begin() + 3));
    // (*vertices)++;

    *(glm::vec3 *)(*vertices + 0) = *(prim.begin() + 0);
    (*vertices)++;
    *(glm::vec3 *)(*vertices + 0) = *(prim.begin() + 1);
    (*vertices)++;
    *(glm::vec3 *)(*vertices + 0) = *(prim.begin() + 2);
    (*vertices)++;
    *(glm::vec3 *)(*vertices + 0) = *(prim.begin() + 3);
    (*vertices)++;
    if (indices == NULL)
        return;
    //std::cout << "Changin" << std::endl;
    unsigned int *idx = *indices;
    idx[0] = new_id + 0;
    idx[1] = new_id + 1;
    idx[2] = new_id + 3;

    idx[3] = new_id + 3;
    idx[4] = new_id + 2;
    idx[5] = new_id + 0;

    *indices += 6;
}

}; // namespace ShapeGenerators

#endif