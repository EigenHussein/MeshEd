#include "../include/CycloidGraphs.h"

Cycloid::ObjectDescription& Cycloid::Grapher2D::
AddGrid(
    const glm::ivec2& blocks,
    const glm::ivec2& subblocks,
    const glm::vec2& seperation,
    const glm::vec4& main_color,
    const glm::vec4& sub_color,
    const glm::vec3& origin,
    const glm::vec3& uGrid,
    const glm::vec3& vGrid
)
{



    glm::vec2 lefttop = { blocks.x * seperation.x,blocks.y * seperation.y };

    for (int xlines = -blocks.x;xlines <= blocks.x;xlines++) {

        glm::vec3 v1 = xlines * seperation.x * uGrid + lefttop.y * vGrid;
        glm::vec3 v2 = xlines * seperation.x * uGrid - lefttop.y * vGrid;

        Indices.push_back(Vertices.size());
        Vertices.push_back({ v1,main_color });
        Indices.push_back(Vertices.size());
        Vertices.push_back({ v2,main_color });

    }



    for (int ylines = -blocks.y;ylines <= blocks.y;ylines++) {

        glm::vec3 v1 = ylines * seperation.y * vGrid + lefttop.x * uGrid;
        glm::vec3 v2 = ylines * seperation.y * vGrid - lefttop.x * uGrid;

        Indices.push_back(Vertices.size());
        Vertices.push_back({ v1,main_color });
        Indices.push_back(Vertices.size());
        Vertices.push_back({ v2,main_color });
    }

    //sub blocks
    glm::vec2 subsep = { seperation.x / subblocks.x,seperation.y / subblocks.y };
    //x sub blocks
    for (int xlines = -blocks.x;xlines < blocks.x;xlines++) {
        for (int i = 1;i < subblocks.x;i++) {
            glm::vec3 tmp = (xlines * seperation.x + i * subsep.x) * uGrid;
            glm::vec3 v1 = tmp + lefttop.y * vGrid;
            glm::vec3 v2 = tmp - lefttop.y * vGrid;

            Indices.push_back(Vertices.size());
            Vertices.push_back({ v1,sub_color });
            Indices.push_back(Vertices.size());
            Vertices.push_back({ v2,sub_color });
        }
    }
    //y sub blocks
    for (int ylines = -blocks.y;ylines < blocks.y;ylines++) {
        for (int i = 1;i < subblocks.y;i++) {
            glm::vec3 tmp = (ylines * seperation.x + i * subsep.y) * vGrid;
            glm::vec3 v1 = tmp + lefttop.y * uGrid;
            glm::vec3 v2 = tmp - lefttop.y * uGrid;

            Indices.push_back(Vertices.size());
            Vertices.push_back({ v1,sub_color });
            Indices.push_back(Vertices.size());
            Vertices.push_back({ v2,sub_color });
        }
    }

    int vertices_count = 4 * (blocks.x + blocks.y + 1) + 2 * (subblocks.x - 1) * 2 * blocks.x + 2 * (subblocks.y - 1) * 2 * blocks.y;
    Objects.push_back(NewObjectDesc(ObjectType::CYC_LINES, vertices_count, vertices_count));
    return Objects.back();

}

Cycloid::ObjectDescription& Cycloid::Grapher2D::Add2DFunction(R1R1 function, float a, float b, int nSteps, const Plane& Plane) {
    for (int i = 0;i < nSteps;i++) {
        float t = a + (b - a) * float(i) / float(nSteps - 1);
        glm::vec3 point_on_graph = Plane.origin + t * Plane.uAxis + function(t) * Plane.vAxis + glm::vec3(0.f, 0.f, -0.001f);
        Indices.push_back(Vertices.size());
        Vertices.push_back({ point_on_graph, { point_on_graph.x,point_on_graph.y,point_on_graph.z,1 } });
    }
    Objects.push_back(NewObjectDesc(CYC_2D_FUNCTION, nSteps, nSteps));
    return Objects.back();
}
