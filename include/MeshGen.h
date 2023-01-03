#ifndef MESHGEN_HH
#define MESHGEN_HH

#include <Meshs.h>

#define t_read(n) auto c##n = std::chrono::high_resolution_clock::now()
#define t_print(a,b) cout<<std::chrono::duration_cast<chrono::microseconds>(c##b-c##a).count()<<endl

typedef glm::vec3(*pfunction)(glm::vec2);

Mesh GetMesh(pfunction f, const glm::vec2& min, const glm::vec2& max, const glm::ivec2& xypolygons) {

    Mesh MyMesh;
    glm::vec2 step = { (max.x - min.x) / float(xypolygons.x),(max.y - min.y) / float(xypolygons.y) };
    for (int y = 0;y < xypolygons.y + 1;y++) {
        for (int x = 0;x < xypolygons.x + 1;x++) {
            glm::vec2 uv = min + glm::vec2(x * step.x, y * step.y);
            glm::vec3 manifold = f(uv);
            MyMesh.pushVertex(manifold);
        }
    }
    const int x_shift = xypolygons.x + 1;
    const int y_shift = xypolygons.y + 1;
    for (int y = 0;y < xypolygons.y;y++) {
        for (int x = 0;x < xypolygons.x;x++) {
            MyMesh.DefineTriangle({
                (y + 1) * x_shift + x + 1,
                (y + 1) * x_shift + x + 0,
                (y + 0) * x_shift + x + 0
                }
            );
            MyMesh.DefineTriangle({
                (y + 0) * x_shift + x + 0,
                (y + 0) * x_shift + x + 1,
                (y + 1) * x_shift + x + 1
                }
            );
        }
    }
    return MyMesh;

}



#endif