#include <Cycloid.h>

namespace Cycloid {
extern GLuint ProgramInUse;

void Grid::SolveGrid(glm::vec3 *buf) {

    const glm::vec2 blocks = {(xblocks / 2), (yblocks / 2)};
    const glm::vec2 scale = {(xblocks / 2) * xStep, (yblocks / 2) * yStep};

    glm::vec3 *ptr = buf;
    for (int xlines = -blocks.x + noPerimeter; xlines <= blocks.x - noPerimeter; xlines++) {

        glm::vec3 v1 = xlines * xStep * uAxis + scale.y * vAxis;
        glm::vec3 v2 = xlines * xStep * uAxis - scale.y * vAxis;

        // cout << "v1 " << v1.x << '\t' << v1.y << '\t' << v1.z << endl;

        *(ptr++) = v1;
        *(ptr++) = v2;
    }

    for (int ylines = -blocks.y + noPerimeter; ylines <= blocks.y - noPerimeter; ylines++) {

        glm::vec3 v1 = ylines * yStep * vAxis + scale.x * uAxis;
        glm::vec3 v2 = ylines * yStep * vAxis - scale.x * uAxis;

        *(ptr++) = v1;
        *(ptr++) = v2;
    }

    // sub blocks

    glm::vec2 subsep = {xStep / xsubblocks, yStep / ysubblocks};
    // x sub blocks
    for (int xlines = -blocks.x; xlines < blocks.x; xlines++) {
        for (int i = 1; i < xsubblocks; i++) {
            glm::vec3 tmp = (xlines * xStep + i * subsep.x) * uAxis;
            glm::vec3 v1 = tmp + scale.y * vAxis;
            glm::vec3 v2 = tmp - scale.y * vAxis;

            *(ptr++) = v1;
            *(ptr++) = v2;
        }
    }

    // y sub blocks
    for (int ylines = -blocks.y; ylines < blocks.y; ylines++) {
        for (int i = 1; i < ysubblocks; i++) {
            glm::vec3 tmp = (ylines * yStep + i * subsep.y) * vAxis;
            glm::vec3 v1 = tmp + scale.x * uAxis;
            glm::vec3 v2 = tmp - scale.x * uAxis;

            *(ptr++) = v1;
            *(ptr++) = v2;
        }
    }

    // int vertices_count = 4 * (blocks.x + blocks.y + 1) + 2 * (subblocks.x - 1) * 2 * blocks.x + 2 * (subblocks.y - 1)
    // * 2 * blocks.y; Objects.push_back(NewObjectDesc(ObjectType::CYC_LINES, vertices_count, vertices_count)); return
    // Objects.back();
}
Grid::Grid(uint nXside, uint nYside, uint subX, uint subY, float x_step, float y_step, GLuint ProgID, bool no_perim)
    : xblocks(2 * nXside), yblocks(2 * nYside), xsubblocks(subX), ysubblocks(subY), main_width(1), sub_width(0.5),
      xStep(x_step), yStep(y_step), main_color({1, 1, 1, 1}), sub_color({1, 1, 1, 0.5}), uAxis({1, 0, 0}),
      vAxis({0, 1, 0}), AB(DT_VERTEX), noPerimeter(no_perim) {

    // body

    ProgramID = ProgID;
    color_uniform = glGetUniformLocation(ProgramID, "uColor");
    main_vertices = 2 * (xblocks + !no_perim) + 2 * (yblocks + !no_perim);
    sub_vertices = 2 * (xblocks) * (xsubblocks - 1) + 2 * (yblocks) * (ysubblocks - 1);
    _buf_size = (main_vertices + sub_vertices) * sizeof(glm::vec3);
}
void Grid::setWidth(float _main, float _sub) {
    main_width = _main;
    sub_width = _sub;
}
void Grid::setColor(const glm::vec4 &_main, const glm::vec4 &_sub) {
    main_color = _main;
    sub_color = _sub;
}
void Grid::setCoords(const glm::vec3 &u, const glm::vec3 &v) {
    uAxis = u;
    vAxis = v;
}
void Grid::Finish() {
    glm::vec3 *vertices = new glm::vec3[main_vertices + sub_vertices];
    SolveGrid(vertices);
    // for (int i = 0;i < 100;i++) {
    //     cout << vertices[i].x << '\t' << vertices[i].y << '\t' << vertices[i].z << endl;
    // }
    AB.AllocateBuffer({{DataBuffer(vertices, _buf_size)}, {GLAttrib(0, 3, 3, 0)}});
    this->AttachBuffer(AB);
    delete[] vertices;
}

void Grid::Draw() {
    //      if (ProgramInUse != ProgramID) {
    // ProgramInUse = ProgramID;
    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    //      }
    glUniform4fv(color_uniform, 1, glm::value_ptr(main_color));
    glLineWidth(main_width);
    glDrawArrays(GL_LINES, 0, main_vertices);
    glUniform4fv(color_uniform, 1, glm::value_ptr(sub_color));
    glLineWidth(sub_width);
    glDrawArrays(GL_LINES, main_vertices, sub_vertices);
}

//====================================Line Function===========================================

void LineFunction::SetMode(const ColorMode CM, const glm::vec4 &solid) {
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);
    if (CM & CM_SINGLE) {
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(solid));
    }
}

void LineFunction::SMode() {

    vertices = new glm::vec4[mLine.steps];
    // cvertices = NULL;
    // Duplicated
    // glm::vec4 *ptr = (glm::vec4 *)vertices;
    // for (int i = 0; i < mLine.steps; i++) {
    //     float t = mLine.a + (mLine.b - mLine.a) * float(i) / float(mLine.steps - 1);
    //     if (!onGrid) {
    //         glm::vec3 res = mLine.function(t);
    //         *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
    //     } else {
    //         glm::vec3 tmp = mLine.function(t);
    //         glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
    //         *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
    //     }
    // }
    ChangeFunction(mLine);
    auto vbuf = (mLine.function != NULL) ? vertices : NULL;
    AB.AllocateBuffer({{DataBuffer(vbuf, mLine.steps * sizeof(glm::vec4))}, {GLAttrib(0, 4, 4, 0)}});
    AttachBuffer(AB);
    // delete[] vertices;
}

void LineFunction::PVMode(const glm::vec4 (*ColorFunc)(float)) {
    vertices = new ColoredVertex[mLine.steps];
    // ColoredVertex *ptr = (ColoredVertex *)vertices;
    // // Duplicated
    // for (int i = 0; i < mLine.steps; i++) {
    //     float t = mLine.a + (mLine.b - mLine.a) * float(i) / float(mLine.steps - 1);
    //     if (!onGrid) {
    //         *(glm::vec3 *)ptr = mLine.function(t);
    //         (ptr++)->color = ColorFunc(t);
    //     } else {
    //         glm::vec3 tmp = mLine.function(t);
    //         tmp = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
    //         *(glm::vec3 *)ptr = tmp;
    //         (ptr++)->color = ColorFunc(t);
    //     }
    // }
    ChangeFunction(mLine);
    auto vbuf = (mLine.function != NULL) ? vertices : NULL;
    AB.AllocateBuffer(
        {{DataBuffer(vbuf, mLine.steps * sizeof(ColoredVertex))}, {GLAttrib(0, 4, 8, 0), GLAttrib(2, 4, 8, 4)}});
    AttachBuffer(AB);
    // delete[] cvertices;
}

void LineFunction::ChangeFunction(const LineDescription &LD) {
    if (LD.function == NULL)
        return;
    assert(LD.steps == mLine.steps);
    // const int offset = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);
    unsigned char *ptr = (unsigned char *)vertices;
    if (this->mCM & (CM_SINGLE | CM_PERPOSITION)) {
        for (int i = 0; i < LD.steps; i++) {
            float t = LD.a + (LD.b - LD.a) * float(i) / float(LD.steps - 1);
            *(glm::vec3 *)ptr = LD.function(t);
            ptr += sizeof(glm::vec4);
        }
    } else if (this->mCM & CM_PERVERTEX) {
        for (int i = 0; i < LD.steps; i++) {
            float t = LD.a + (LD.b - LD.a) * float(i) / float(LD.steps - 1);

            *(glm::vec3 *)ptr = LD.function(t);
            ptr += sizeof(ColoredVertex);
        }
    }
}

LineFunction::LineFunction(const LineDescription &L, const ColorMode CM, const GLuint ProgID, const Grid *grid_ref,
                           const glm::vec4 (*ColorFunc)(float))
    : AB(DT_VERTEX), mLine(L), onGrid(grid_ref != NULL), mCM(CM) {
    LineWidth = 1.f;
    if (onGrid) {
        coords[0] = grid_ref->uAxis;
        coords[1] = grid_ref->vAxis;
        coords[2] = glm::cross(grid_ref->uAxis, grid_ref->vAxis);
    }

    ProgramID = (ProgID == -1) ? DefaultProgram() : ProgID;
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);

    if (CM & (CM_SINGLE | CM_PERPOSITION)) {
        glm::vec4 SolidColor = (ColorFunc == 0) ? glm::vec4(1, 1, 1, 1) : ColorFunc(0);
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(SolidColor));

        SMode();
    } else if (CM & CM_PERVERTEX) {
        if (ColorFunc != 0) {
            PVMode(ColorFunc);
        } else {
            PVMode([](float t) -> const glm::vec4 { return glm::vec4(t, t, t, 1); });
        }
    }
}

void LineFunction::UpdateFunction(const LineDescription &NewLine) {
    ChangeFunction(NewLine);
    UpdateVertices();
}
void LineFunction::UpdateVertices() {
    if (mCM & (CM_PERPOSITION | CM_SINGLE)) {
        SceneData->UpdateBuffer(0, mLine.steps * sizeof(glm::vec4), vertices);
    } else if (mCM & CM_PERVERTEX) {
        SceneData->UpdateBuffer(0, mLine.steps * sizeof(ColoredVertex), vertices);
    }
}
void LineFunction::Draw2(float linewidth, const int points) {
    glLineWidth(linewidth);
    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    // std::cout << ((points > -1) ? points : mLine.steps) << std::endl;
    glDrawArrays(GL_LINE_STRIP, 0, (points > -1) ? points : mLine.steps);
}
void LineFunction::Draw() {
    // if (ProgramInUse != ProgramID) {
    // ProgramInUse = ProgramID;
    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    //}
    glDrawArrays(GL_LINE_STRIP, 0, mLine.steps);
}

//================================== Chart Funcion ==================================

ChartFunction::ChartFunction(const LineDescription &L, const ColorMode CM, const GLuint ProgID, const Grid *grid_ref,
                             const glm::vec4 (*ColorFunc)(float))
    : AB(DT_ELEMENT), mLine(L), onGrid(grid_ref != NULL), mCM(CM), nVertices(4 * (L.steps - 1)),
      nIndices(6 * (L.steps - 1)) {
    LineWidth = 1.f;
    if (onGrid) {
        coords[0] = grid_ref->uAxis;
        coords[1] = grid_ref->vAxis;
        coords[2] = glm::cross(grid_ref->uAxis, grid_ref->vAxis);
    }

    ProgramID = (ProgID == -1) ? DefaultProgram() : ProgID;
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);

    if (CM & (CM_SINGLE | CM_PERPOSITION)) {
        glm::vec4 SolidColor = (ColorFunc == 0) ? glm::vec4(1, 1, 1, 1) : ColorFunc(0);
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(SolidColor));

        SMode();
    } else if (CM & CM_PERVERTEX) {
        if (ColorFunc != 0) {
            PVMode(ColorFunc);
        } else {
            PVMode([](float t) -> const glm::vec4 { return glm::vec4(t, t, t, 1); });
        }
    }
}

void ChartFunction::SetMode(const ColorMode CM, const glm::vec4 &solid) {
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);
    if (CM & CM_SINGLE) {
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(solid));
    }
}

void ChartFunction::SMode() {

    vertices = new glm::vec4[nVertices];
    // const unsigned int nIndices = (nVertices / 4) * 6;
    indices = new unsigned int[nIndices];
    unsigned int *idx = indices;
    for (int i = 0; i < nIndices / 6; i++) {
        idx[0] = 4 * i + 0;
        idx[1] = 4 * i + 1;
        idx[2] = 4 * i + 3;

        idx[3] = 4 * i + 3;
        idx[4] = 4 * i + 2;
        idx[5] = 4 * i + 0;
        idx += 6;
    }
    ChangeFunction(mLine);
    auto vbuf = (mLine.function != NULL) ? vertices : NULL;
    AB.AllocateBuffer({{DataBuffer(vbuf, 6 * (mLine.steps - 1) * sizeof(glm::vec4))}, {GLAttrib(0, 4, 4, 0)}});
    AB.AllocateElements(indices, nIndices * sizeof(unsigned int));
    AttachBuffer(AB);
    // delete[] vertices;
}

void ChartFunction::PVMode(const glm::vec4 (*ColorFunc)(float)) {
    vertices = new ColoredVertex[nVertices];
    // const unsigned int nIndices = (nVertices / 4) * 6;
    indices = new unsigned int[nIndices];

    // quad indices
    unsigned int *idx = indices;
    for (int i = 0; i < nIndices / 6; i++) {
        idx[0] = 4 * i + 0;
        idx[1] = 4 * i + 1;
        idx[2] = 4 * i + 3;

        idx[3] = 4 * i + 3;
        idx[4] = 4 * i + 2;
        idx[5] = 4 * i + 0;
        idx += 6;
    }
    ChangeFunction(mLine);
    auto vbuf = (mLine.function != NULL) ? vertices : NULL;
    AB.AllocateBuffer(
        {{DataBuffer(vbuf, mLine.steps * sizeof(ColoredVertex))}, {GLAttrib(0, 4, 8, 0), GLAttrib(2, 4, 8, 4)}});
    AB.AllocateElements(indices, nIndices * sizeof(unsigned int));
    AttachBuffer(AB);
    // delete[] cvertices;
}

void ChartFunction::ChangeFunction(const LineDescription &LD) {
    if (LD.function == NULL)
        return;
    mLine = LD;
    assert(LD.steps == mLine.steps);
    // const int offset = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);

    if (this->mCM & (CM_SINGLE | CM_PERPOSITION)) {
        glm::vec4 *ptr = (glm::vec4 *)vertices;
        for (int i = 0; i < LD.steps - 1; i++) {
            float t1 = LD.a + (LD.b - LD.a) * float(i) / float(LD.steps - 1);
            float t2 = LD.a + (LD.b - LD.a) * float(i + 1) / float(LD.steps - 1);
            glm::vec3 p1 = mLine.function(t1), p2 = mLine.function(t2);
            float slope = (p2.y - p1.y) / (t2 - t1);
            // glm::vec3 p[4] = {{p1.x, 0, 0}, {p2.x, 0, 0}, {p1.x, slope, 0}, {p2.x, slope, 0}};
            ShapeGenerators::Quad({{p1.x, 0, 0}, {p2.x, 0, 0}, {p1.x, slope, 0}, {p2.x, slope, 0}}, &ptr);
        }
    } else if (this->mCM & CM_PERVERTEX) {
        ColoredVertex *ptr = (ColoredVertex *)vertices;
        for (int i = 0; i < LD.steps - 1; i++) {
            float t1 = LD.a + (LD.b - LD.a) * float(i) / float(LD.steps - 1);
            float t2 = LD.a + (LD.b - LD.a) * float(i + 1) / float(LD.steps - 1);
            glm::vec3 p1 = mLine.function(t1), p2 = mLine.function(t2);
            float slope = (p2.y - p1.y) / (t2 - t1);
            // glm::vec3 p[4] = {{p1.x, 0, 0}, {p2.x, 0, 0}, {p1.x, slope, 0}, {p2.x, slope, 0}};
            ShapeGenerators::Quad({{p1.x, 0, 0}, {p2.x, 0, 0}, {p1.x, slope, 0}, {p2.x, slope, 0}}, &ptr);
        }
    }
}

void ChartFunction::UpdateFunction(const LineDescription &NewLine) {
    ChangeFunction(NewLine);
    UpdateVertices();
}
void ChartFunction::UpdateVertices() {
    if (mCM & (CM_PERPOSITION | CM_SINGLE)) {
        SceneData->UpdateBuffer(0, nVertices * sizeof(glm::vec4), vertices);
    } else if (mCM & CM_PERVERTEX) {
        SceneData->UpdateBuffer(0, nVertices * sizeof(ColoredVertex), vertices);
    }
}
void ChartFunction::Draw2(float linewidth, const int points) {
    glLineWidth(linewidth);
    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    // std::cout << ((points > -1) ? points : mLine.steps) << std::endl;
    glDrawArrays(GL_LINE_STRIP, 0, (points > -1) ? points : mLine.steps);
}
void ChartFunction::Draw() {
    // if (ProgramInUse != ProgramID) {
    // ProgramInUse = ProgramID;
    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SceneData->EBO);
    //}
    // glDrawArrays(GL_LINE_STRIP, 0, mLine.steps);
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);
}

//=================================== 3D Graphs ======================================

void Graph3D::SetMode(const ColorMode CM, const glm::vec4 &solid) {
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);
    if (CM & CM_SINGLE) {
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(solid));
    }
}
void Graph3D::GenIndices() {
    nIndices = 6 * (mSurface.step_uv.x - 1) * (mSurface.step_uv.y - 1);
    indices = new unsigned int[nIndices];
    unsigned int *ptr = indices;
    for (int v = 0; v < mSurface.step_uv.y - 1; v++) {
        for (int u = 0; u < mSurface.step_uv.x - 1; u++) {
            ptr[0] = (u + 0) + mSurface.step_uv.x * (v + 0);
            ptr[1] = (u + 1) + mSurface.step_uv.x * (v + 0);
            ptr[2] = (u + 1) + mSurface.step_uv.x * (v + 1);
            ptr[3] = (u + 1) + mSurface.step_uv.x * (v + 1);
            ptr[4] = (u + 0) + mSurface.step_uv.x * (v + 1);
            ptr[5] = (u + 0) + mSurface.step_uv.x * (v + 0);
            ptr += 6;
        }
    }
}
void Graph3D::SMode() {

    nVertices = mSurface.step_uv.x * mSurface.step_uv.y;
    vertices = new glm::vec4[nVertices];
    cvertices = NULL;
    const glm::vec2 d_uv = {(mSurface.b.x - mSurface.a.x) / float(mSurface.step_uv.x - 1),
                            (mSurface.b.y - mSurface.a.y) / float(mSurface.step_uv.y - 1)};

    glm::vec4 *ptr = vertices;
    for (int v = 0; v < mSurface.step_uv.y; v++) {
        float cv = mSurface.a.y + d_uv.y * v;
        for (int u = 0; u < mSurface.step_uv.x; u++) {
            float cu = mSurface.a.x + d_uv.x * u;

            if (!onGrid) {
                glm::vec3 res = mSurface.function({cu, cv});
                *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
            } else {
                glm::vec3 tmp = mSurface.function({cu, cv});
                glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
                *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
            }
        }
    }

    AB.AllocateBuffer({{DataBuffer(vertices, nVertices * sizeof(glm::vec4))}, {GLAttrib(0, 4, 4, 0)}});
    AB.AllocateElements(indices, nIndices * sizeof(unsigned int));
    AttachBuffer(AB);
    // delete[] vertices;
}

void Graph3D::PVMode(const glm::vec4 (*ColorFunc2D)(const glm::vec2 &)) {
    nVertices = mSurface.step_uv.x * mSurface.step_uv.y;
    cvertices = new ColoredVertex[nVertices];
    vertices = NULL;
    const glm::vec2 d_uv = {(mSurface.b.x - mSurface.a.x) / float(mSurface.step_uv.x - 1),
                            (mSurface.b.y - mSurface.a.y) / float(mSurface.step_uv.y - 1)};

    ColoredVertex *ptr = cvertices;
    for (int v = 0; v < mSurface.step_uv.y; v++) {
        float cv = mSurface.a.y + d_uv.y * v;
        for (int u = 0; u < mSurface.step_uv.x; u++) {
            float cu = mSurface.a.x + d_uv.x * u;

            if (!onGrid) {
                glm::vec3 res = mSurface.function({cu, cv});
                //   std::cout << "KEK " << res.x << '\t' << res.y << '\t' << res.z << std::endl;
                ptr->color = ColorFunc2D({cu, cv});
                ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                ptr++;
            } else {
                glm::vec3 tmp = mSurface.function({cu, cv});
                glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];

                ptr->color = ColorFunc2D({cu, cv});
                ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                ptr++;
            }
        }
    }

    AB.AllocateBuffer(
        {{DataBuffer(cvertices, nVertices * sizeof(ColoredVertex))}, {GLAttrib(0, 4, 8, 0), GLAttrib(2, 4, 8, 4)}});

    AB.AllocateElements(indices, nIndices * sizeof(unsigned int));
    AttachBuffer(AB);
    // delete[] cvertices;
}

void Graph3D::ChangeFunction(const SurfaceDescription &SD) {
    assert((SD.step_uv.x == mSurface.step_uv.x) && (SD.step_uv.y == mSurface.step_uv.y));
    const glm::vec2 d_uv = {(mSurface.b.x - mSurface.a.x) / float(mSurface.step_uv.x - 1),
                            (mSurface.b.y - mSurface.a.y) / float(mSurface.step_uv.y - 1)};

    if (this->mCM & (CM_SINGLE | CM_PERPOSITION)) {

        glm::vec4 *ptr = vertices;
        for (int v = 0; v < mSurface.step_uv.y; v++) {
            float cv = mSurface.a.y + d_uv.y * v;
            for (int u = 0; u < mSurface.step_uv.x; u++) {
                float cu = mSurface.a.x + d_uv.x * u;

                if (!onGrid) {
                    glm::vec3 res = SD.function({cu, cv});
                    *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
                } else {
                    glm::vec3 tmp = SD.function({cu, cv});
                    glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
                    *(ptr++) = glm::vec4(res.x, res.y, res.z, 0.f);
                }
            }
        }

    } else if (this->mCM & CM_PERVERTEX) {
        const glm::vec2 d_uv = {(mSurface.b.x - mSurface.a.x) / float(mSurface.step_uv.x - 1),
                                (mSurface.b.y - mSurface.a.y) / float(mSurface.step_uv.y - 1)};

        ColoredVertex *ptr = cvertices;
        for (int v = 0; v < mSurface.step_uv.y; v++) {
            float cv = mSurface.a.y + d_uv.y * v;
            for (int u = 0; u < mSurface.step_uv.x; u++) {
                float cu = mSurface.a.x + d_uv.x * u;

                if (!onGrid) {
                    glm::vec3 res = mSurface.function({cu, cv});
                    // ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    *(glm::vec3 *)ptr = res;
                    ptr++;
                } else {
                    glm::vec3 tmp = mSurface.function({cu, cv});
                    glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
                    // ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    *(glm::vec3 *)ptr = res;
                    ptr++;
                }
            }
        }
    }
}

Graph3D::~Graph3D() {
    delete[] indices;
    if (vertices != NULL)
        delete[] vertices;
    if (cvertices != NULL)
        delete[] cvertices;
}
Graph3D::Graph3D(const SurfaceDescription &L, const ColorMode CM, const GLuint ProgID, const Grid *grid_ref,
                 const glm::vec4 (*ColorFunc2D)(const glm::vec2 &))
    : AB(DT_ELEMENT), mSurface(L), onGrid(grid_ref != NULL), mCM(CM) {
    if (onGrid) {
        coords[0] = grid_ref->uAxis;
        coords[1] = grid_ref->vAxis;
        coords[2] = glm::cross(grid_ref->uAxis, grid_ref->vAxis);
    }

    ProgramID = (ProgID == -1) ? DefaultProgram() : ProgID;
    glUseProgram(ProgramID);
    glUniform1ui(glGetUniformLocation(ProgramID, "ColorMode"), CM);

    if (CM & (CM_SINGLE | CM_PERPOSITION)) {
        glm::vec4 SolidColor = (ColorFunc2D == 0) ? glm::vec4(1, 1, 1, 1) : ColorFunc2D({0, 0});
        glUniform4fv(glGetUniformLocation(ProgramID, "SolidColor"), 1, glm::value_ptr(SolidColor));
        GenIndices();
        SMode();

    } else if (CM & CM_PERVERTEX) {
        if (ColorFunc2D != 0) {
            GenIndices();
            PVMode(ColorFunc2D);

        } else {
            GenIndices();
            PVMode([](const glm::vec2 &uv) -> const glm::vec4 { return glm::vec4(uv.x, uv.y, uv.x + uv.y, 1); });
        }
    }
}

void Graph3D::UpdateFunction(const SurfaceDescription &NewSurface) {
    ChangeFunction(NewSurface);
    if (mCM & (CM_PERPOSITION | CM_SINGLE)) {
        SceneData->UpdateBuffer(0, nVertices * sizeof(vertices[0]), vertices);
    } else if (mCM & CM_PERVERTEX) {
        SceneData->UpdateBuffer(0, nVertices * sizeof(cvertices[0]), cvertices);
    }
}

void Graph3D::Draw() {

    glUseProgram(ProgramID);
    glBindVertexArray(SceneData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SceneData->EBO);

    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);
}

void Graph3D::softUpdate(ColoredVertex *vbuf, const SurfaceDescription &NewSurface, const ColorUpdateFunc ColorFunc2D,
                         const Grid *grid_ref) {

    glm::vec3 coords[3];
    if (grid_ref != NULL) {
        coords[0] = grid_ref->uAxis;
        coords[1] = grid_ref->vAxis;
        coords[2] = glm::cross(grid_ref->uAxis, grid_ref->vAxis);
    }

    const glm::vec2 d_uv = {(NewSurface.b.x - NewSurface.a.x) / float(NewSurface.step_uv.x - 1),
                            (NewSurface.b.y - NewSurface.a.y) / float(NewSurface.step_uv.y - 1)};

    ColoredVertex *ptr = vbuf;
    if (ColorFunc2D != NULL) {
        for (int v = 0; v < NewSurface.step_uv.y; v++) {
            float cv = NewSurface.a.y + d_uv.y * v;
            for (int u = 0; u < NewSurface.step_uv.x; u++) {
                float cu = NewSurface.a.x + d_uv.x * u;

                if (grid_ref == NULL) {
                    glm::vec3 res = NewSurface.function({cu, cv});
                    //   std::cout << "KEK " << res.x << '\t' << res.y << '\t' << res.z << std::endl;
                    ptr->color = ColorFunc2D({cu, cv});
                    ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    ptr++;
                } else {
                    glm::vec3 tmp = NewSurface.function({cu, cv});
                    glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];

                    ptr->color = ColorFunc2D({cu, cv});
                    ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    ptr++;
                }
            }
        }
    } else if (ColorFunc2D == NULL) {
        for (int v = 0; v < NewSurface.step_uv.y; v++) {
            float cv = NewSurface.a.y + d_uv.y * v;
            for (int u = 0; u < NewSurface.step_uv.x; u++) {
                float cu = NewSurface.a.x + d_uv.x * u;

                if (grid_ref == NULL) {
                    glm::vec3 res = NewSurface.function({cu, cv});
                    //   std::cout << "KEK " << res.x << '\t' << res.y << '\t' << res.z << std::endl;
                    ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    ptr++;
                } else {
                    glm::vec3 tmp = NewSurface.function({cu, cv});
                    glm::vec3 res = tmp.x * coords[0] + tmp.y * coords[1] + tmp.z * coords[2];
                    ptr->vertex = glm::vec4(res.x, res.y, res.z, 0.f);
                    ptr++;
                }
            }
        }
    }
}
//==========================================Function Sweep=============================================

FunctionSweep::FunctionSweep(LineFunction *Line, const LineFunction::LineDescription &L1,
                             const LineFunction::LineDescription &L2)
    : ComputeSweep("../shaders/Cycloid/c_linesweep.glsl"), LF(Line), G3D(NULL) {
    ComputeSweep.AddSSBO(LF->SceneData->GBuf.buf_size, 1);
    ComputeSweep.AddSSBO(LF->SceneData->GBuf.buf_size, 2);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LF->SceneData->GBuf.buf_idx);
    ColoredVertex *cvertices = (ColoredVertex *)calloc(LF->mLine.steps, sizeof(ColoredVertex));

    for (int i = 0; i < L1.steps; i++) {
        float t = L1.a + (L1.b - L1.a) * float(i) / float(L1.steps - 1);
        *(glm::vec3 *)&cvertices[i].vertex = L1.function(t);
    }
    ComputeSweep.SetData(0, cvertices, LF->SceneData->GBuf.buf_size, 0);

    for (int i = 0; i < L2.steps; i++) {
        float t = L2.a + (L2.b - L2.a) * float(i) / float(L2.steps - 1);
        *(glm::vec3 *)&cvertices[i].vertex = L2.function(t);
    }

    ComputeSweep.SetData(1, cvertices, LF->SceneData->GBuf.buf_size, 0);

    free(cvertices);
}

FunctionSweep::FunctionSweep(Graph3D *g3d, const Graph3D::SurfaceDescription &S1, const Graph3D::SurfaceDescription &S2)
    : ComputeSweep("../shaders/Cycloid/c_linesweep.glsl"), G3D(g3d), LF(NULL) {
    ComputeSweep.AddSSBO(g3d->SceneData->GBuf.buf_size, 1);
    ComputeSweep.AddSSBO(g3d->SceneData->GBuf.buf_size, 2);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g3d->SceneData->GBuf.buf_idx);

    ColoredVertex *cvertices = (ColoredVertex *)calloc(g3d->nVertices, sizeof(ColoredVertex));

    Graph3D::softUpdate(cvertices, S1);

    ComputeSweep.SetData(0, cvertices, g3d->SceneData->GBuf.buf_size, 0);
    Graph3D::softUpdate(cvertices, S2);

    ComputeSweep.SetData(1, cvertices, g3d->SceneData->GBuf.buf_size, 0);

    free(cvertices);
}

void FunctionSweep::lerp(float t) {
    int nV = 0;
    if (LF == NULL && G3D != NULL) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, G3D->SceneData->GBuf.buf_idx);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ComputeSweep.SSBOs[0].buf_idx);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ComputeSweep.SSBOs[1].buf_idx);
        nV = G3D->nVertices;

    } else if (LF != NULL) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LF->SceneData->GBuf.buf_idx);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ComputeSweep.SSBOs[0].buf_idx);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ComputeSweep.SSBOs[1].buf_idx);
        nV = LF->mLine.steps;
        // std::cout << nV << std::endl;
    }
    glUseProgram(ComputeSweep.ProgramID);
    glUniform1f(glGetUniformLocation(ComputeSweep.ProgramID, "t"), t);
    glDispatchCompute(nV / 8, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

}; // namespace Cycloid
