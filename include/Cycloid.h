#ifndef CYCLOID_GRAPHS_HH
#define CYCLOID_GRAPHS_HH
#include <Core2.h>
#include <Fields/ComputeShader.h>
#include <Program.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <ShapeUtil.h>
/// @brief Template function to convert spherical coordinates to cartesian coordinates
/// @tparam sphericalfunc A pointer to polar function r(rho,theta)
/// @param s Spherical coordinates (rho,theta) evaluated in sphericalfunc
/// @return Returns cartesian coordinates
template <const float (*sphericalfunc)(const float, const float)> const glm::vec3 sphe2cart(const glm::vec2 &s) {
    const float r = sphericalfunc(s[0], s[1]);
    return {r * sin(s[0]) * cos(s[1]), r * sin(s[0]) * sin(s[1]), r * cos(s[0])};
}

namespace Cycloid {
typedef float (*R1R1)(float);
typedef glm::vec2 (*R1R2)(float);
typedef glm::vec3 (*R1R3)(float);
typedef const glm::vec3 (*R2R3)(const glm::vec2 &);
extern GLuint ProgramInUse;

/// @brief Color mode of vertices size{4f,8f,4f}
enum ColorMode {
    CM_SINGLE = 1,
    CM_PERVERTEX = 2,
    CM_PERPOSITION = 4
};

/// @brief Line function class
class LineFunction;

/// @brief class for 3d graphs
class Graph3D;

/// @brief Subclass of "Program" that draw grids
class Grid : public Program {

  private:
    int xblocks;
    int yblocks;
    int xsubblocks;
    int ysubblocks;
    float main_width;
    float sub_width;
    float xStep;
    float yStep;
    glm::vec4 main_color;
    glm::vec4 sub_color;
    int color_uniform;
    bool noPerimeter;

    ArrayBuffer AB;
    unsigned int _buf_size;

    /// @brief Initialize Grid's vertex buffer
    /// @param buf Pointer to pre-allocated buffer
    void SolveGrid(glm::vec3 *buf);

  protected:
    /// @brief Local plane axis
    glm::vec3 uAxis;
    /// @brief Local plane axis
    glm::vec3 vAxis;
    // tmp
    unsigned int main_vertices;
    unsigned int sub_vertices;

  public:
    friend class LineFunction;
    friend class ChartFunction;
    friend class Graph3D;

    /// @brief Default grid shader program
    /// @return Shader program ID
    static GLuint DefaultProgram() {
        return GL::GetProgram("../shaders/Cycloid/vertex.glsl", "../shaders/Cycloid/f_grid.glsl", NULL);
    }

    /// @brief Grid constructor
    /// @example
    /// auto GG = Grid(...);
    /// GG.setColor(0xFFFFFFFF, 0xFFFFFF7F);
    /// GG.setWidth(1.5, 1);
    /// GG.Finish();
    /// @param nXside Number of quads in every positive relative x (x2 for both sides)
    /// @param nYside Number of quads in every positive relative y (x2 for both sides)
    /// @param subX Number of (x) sub-quads
    /// @param subY Number of (y) sub-quads
    /// @param x_step Width of quad
    /// @param y_step Height of quad
    /// @param ProgID Shader ID
    Grid(uint nXside, uint nYside, uint subX, uint subY, float x_step = 1.f, float y_step = 1.f,
         GLuint ProgID = DefaultProgram(), bool no_perm = false);

    /// @brief Set width of grid and subgrid
    /// @param _main Width (in pixels) of grid lines
    /// @param _sub  Width (in pixels) of sub-grid lines
    void setWidth(float _main, float _sub);

    /// @brief Set grid color
    /// @param _main Main-grid's [0,1]*RBGA color
    /// @param _sub Sub-grid's  [0,1]*RBGA color
    void setColor(const glm::vec4 &_main, const glm::vec4 &_sub);

    /// @brief Set grid color
    /// @param _main Main-grid's RBGA color int32 (0xRRGGBBAA)
    /// @param _sub Sub-grid's  RBGA color int32 (0xRRGGBBAA)
    void setColor(const int _main, const int _sub) {
        main_color = {float((_main >> 24) & 0xFF) / 255.0, float((_main >> 16) & 0xFF) / 255.0,
                      float((_main >> 8) & 0xFF) / 255.0, float((_main >> 0) & 0xFF) / 255.0};
        sub_color = {float((_sub >> 24) & 0xFF) / 255.0, float((_sub >> 16) & 0xFF) / 255.0,
                     float((_sub >> 8) & 0xFF) / 255.0, float((_sub >> 0) & 0xFF) / 255.0};
    }

    /// @brief Set Global Coordinates
    /// @param u First Coordinate (X-Axis)
    /// @param v Seconds Coordinate (Y-Axis)
    void setCoords(const glm::vec3 &u, const glm::vec3 &v);

    /// @brief Allocate and initialize vertex buffer and ...
    void Finish();

    /// @brief Draw grid
    void Draw() override;
};

#define GL_CORE_DEFAULT(w, h)                                                                                          \
    GL::Core(                                                                                                          \
        "Grapher", {w, h}, GL::Core::InitType::IT_OpenGL_ImGui,                                                        \
        [](GLFWwindow *wnd) { glfwSetKeyCallback(wnd, KeyCallback); },                                                 \
        [](ImGuiIO *ptr) { ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }, GLFW_OPENGL_COMPAT_PROFILE)

/// @brief Struct to handle per-vertex colors size{8*f}
struct ColoredVertex {
    glm::vec4 vertex;
    glm::vec4 color;
    ColoredVertex(const glm::vec4 &v = {}, const glm::vec4 &c = {}) : vertex(v), color(c) {
    }
};

class FunctionSweep;

class LineFunction : public Program {

  public:
    struct LineDescription {
        R1R3 function;
        float a;
        float b;
        int steps;
        LineDescription() {
        }
        /// @brief Discribe path function using intervals, number of steps and a function
        /// @param f A pointer of path function (float -> glm::vec3)
        /// @param t1 The first value in the interval
        /// @param t2 The last value in the interval
        /// @param _steps Number of steps
        LineDescription(R1R3 f, float t1, float t2, int _steps) : function(f), a(t1), b(t2), steps(_steps) {
        }
    };
    struct SubFunction {
        int offset;
        int length;
        glm::vec3 spatial_shift;

        /// @brief Sub function description
        /// @param _offset buffer offset (measured in the # of elements to skip)
        /// @param _vertices_count Number of vertices to draw
        /// @param _spatial_shift Translation vector
        SubFunction() : offset(0), length(0), spatial_shift({}) {
        }
        SubFunction(const int _offset, const int _vertices_count, const glm::vec3 &_spatial_shift = {})
            : offset(_offset), length(_vertices_count), spatial_shift(_spatial_shift) {
        }
    };

  private:
    ArrayBuffer AB;
    LineDescription mLine;
    float LineWidth;
    ColorMode mCM;
    const bool onGrid;
    glm::vec3 coords[3];

    void *vertices;

    bool MutiFun = false;

    std::vector<int> MultipleFunctions;

    /// @brief Set Colour mode
    /// @param CM Color mode (Solid, Per-vertex, Per-Position)
    /// @param solid Solid color if the choosen mode is solid
    void SetMode(const ColorMode CM, const glm::vec4 &solid = {0.1, 0.3, 0.23, 1});
    // solid color mode

    /// @brief Init solid colored line
    void SMode();
    // Not Tested

    /// @brief Init Per-vertex curve
    /// @param ColorFunc A pointer to color function (float -> RGBA)
    void PVMode(const glm::vec4 (*ColorFunc)(float));

    /// @brief Change current function
    /// @param LD Line description for the new function
    void ChangeFunction(const LineDescription &LD);

  public:
    friend class FunctionSweep;

    static GLuint DefaultProgram() {
        return GL::GetProgram("../shaders/Cycloid/vertex.glsl", "../shaders/Cycloid/f_function.glsl", NULL);
    }
    ~LineFunction() {
        if (vertices != NULL)
            delete[] vertices;
        //    if (cvertices != NULL)delete[] cvertices;
    }
    /// @brief Initialize path function
    /// @param L Line description
    /// @param CM Color Mode
    /// @param ProgID Shader Program ID
    /// @param grid_ref If the function is relative to grid (null for default)
    /// @param ColorFunc Color function (Set for CM_PERVERTEX color mode)
    LineFunction(const LineDescription &L, const ColorMode CM, const GLuint ProgID = DefaultProgram(),
                 const Grid *grid_ref = NULL, const glm::vec4 (*ColorFunc)(float) = NULL);

    /// @brief Update the current function
    /// @param NewLine Decription for the new line
    void UpdateFunction(const LineDescription &NewLine);

    /// @brief Update GPU buffer (copy CPU buffer)
    void UpdateVertices();

    /// @brief Draw the function
    /// @param linewidth The width in pixels
    /// @param points Number of verices to draw
    void Draw2(float linewidth, const int points = -1);

    /// @brief Default vanella draw call
    void Draw() override;

    // void MultipleFunctions(const std::vector<int> &offsets) {
    //     int acc = 0;
    //     for (int i = 0; i < offsets.size(); i++) {
    //         acc += offsets[i];
    //     }
    // }
    // void DrawMultipleFunctions(float linewidth = 1.5) {
    //     if (MutiFun) {

    //         glLineWidth(linewidth);
    //         glUseProgram(ProgramID);
    //         glBindVertexArray(SceneData->VAO);
    //         int acc = 0;
    //         for (int i = 0; i < MultipleFunctions.size(); i++) {
    //             const auto k = MultipleFunctions[i];
    //             glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    //             // std::cout << ((points > -1) ? points : mLine.steps) << std::endl;
    //             glDrawArrays(GL_LINE_STRIP, acc, k);
    //             acc += k;
    //         }
    //     }
    // }

    void DrawMulFunc(const std::vector<SubFunction> &sub_functions, const float linewidth = 1.5) {

        const int n_functions = sub_functions.size();

        glLineWidth(linewidth);
        glUseProgram(ProgramID);
        glBindVertexArray(SceneData->VAO);

        for (int i = 0; i < n_functions; ++i) {
            glUniform3fv(0, 1, glm::value_ptr(sub_functions[i].spatial_shift));
            glDrawArrays(GL_LINE_STRIP, sub_functions[i].offset, sub_functions[i].length);
        }
    }

    GPUBuffer GetVBO() const {
        return AB.GBuf;
    }

    /// @brief Append new values to the function (Shift old then add new)
    /// @note SLOW
    /// @param new_vals A pointer to a buffer contains vertices
    /// @param count The number of vertices to append
    void AppendValue(const glm::vec3 *new_vals, const int count) {
        // const int stride = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);
        if (mCM == CM_PERVERTEX) {
            ColoredVertex *ptr = (ColoredVertex *)vertices;
            for (int i = 0; i < mLine.steps - count; i++) {
                ptr[i].vertex = ptr[i + count].vertex;
            }
            const int offset = mLine.steps - count;
            for (int i = 0; i < count; i++) {
                *(glm::vec3 *)&ptr[i + offset] = new_vals[i];
            }
            SceneData->UpdateBuffer(0, mLine.steps * sizeof(ColoredVertex), vertices);
        } else {
            glm::vec4 *ptr = (glm::vec4 *)vertices;
            for (int i = 0; i < mLine.steps - count; i++) {
                ptr[i] = ptr[i + count];
            }
            const int offset = mLine.steps - count;
            for (int i = 0; i < count; i++) {
                *(glm::vec3 *)&ptr[i + offset] = new_vals[i];
            }
            SceneData->UpdateBuffer(0, mLine.steps * sizeof(glm::vec4), vertices);
        }
    }

    /// @brief Append new values to the function (Shift old then add new) using memory mapping
    /// @note FASTER than AppendValue()
    /// @param new_vals A pointer to a buffer contains vertices
    /// @param count The number of vertices to append
    void AppendValue2(const glm::vec4 *new_vals, const int count) {
        const int stride = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);
        char *map = (char *)glMapNamedBuffer(SceneData->GBuf.buf_idx, GL_READ_WRITE);
        if (count < mLine.steps)
            memmove(map, map + stride * count, (this->mLine.steps - count) * stride);
        memcpy(map + stride * (this->mLine.steps - count), new_vals, count * stride);
        glUnmapNamedBuffer(SceneData->GBuf.buf_idx);
    }

    /// @brief Get a pointer to CPU buffer
    /// @return return a void pointer
    void *GetVertices() const {
        return vertices;
    }
};

//======================================= 3D Function ===========================================

class Graph3D : public Program {
  public:
    typedef const glm::vec4 (*ColorUpdateFunc)(const glm::vec2 &);
    struct SurfaceDescription {
        R2R3 function;
        glm::vec2 a;
        glm::vec2 b;
        glm::ivec2 step_uv;
        SurfaceDescription() {
        }
        /// @brief R3 Surface description
        /// @param f Surface's parametric function
        /// @param uv1 First variable domain
        /// @param uv2 Second variable domain
        /// @param _steps Number of steps in the two directions
        SurfaceDescription(R2R3 f, const glm::vec2 &uv1, const glm::vec2 &uv2, glm::ivec2 _steps)
            : function(f), a(uv1), b(uv2), step_uv(_steps) {
        }
        const int nVertices() const {
            return step_uv.x * step_uv.y;
        }
    };

    /// @brief Static function to update Buffer using SurfaceDescription and Colour function
    /// @param vbuf A pointer to buffer
    /// @param NewSurface The new surface
    /// @param ColorFunc2D Color function
    /// @param grid_ref Reference grid - x,y,z by default
    static void softUpdate(ColoredVertex *vbuf, const SurfaceDescription &NewSurface,
                           const ColorUpdateFunc ColorFunc2D = NULL, const Grid *grid_ref = NULL);

  private:
    ArrayBuffer AB;
    SurfaceDescription mSurface;
    ColorMode mCM;
    const bool onGrid;
    glm::vec3 coords[3];

    // mesh inforamtion
    ColoredVertex *cvertices;
    glm::vec4 *vertices;
    unsigned int *indices;

    int nVertices;
    int nIndices;

    void SetMode(const ColorMode CM, const glm::vec4 &solid = {0.1, 0.3, 0.23, 1});
    // solid color mode
    void SMode();
    // Not Tested
    void PVMode(ColorUpdateFunc ColorFunc2D);
    void GenIndices();
    void ChangeFunction(const SurfaceDescription &LD);

  public:
    friend class FunctionSweep;
    static GLuint DefaultProgram() {
        return GL::GetProgram("../shaders/Cycloid/vertex.glsl", "../shaders/Cycloid/f_function.glsl", NULL);
    }
    ~Graph3D();

    /// @brief Subclass of "Program" to graph 3D surfaces
    /// @param L SurfaceDescription
    /// @param CM Surface's Color mode
    /// @param ProgID Shader Program ID
    /// @param grid_ref Reference grid
    /// @param ColorFunc2D A pointer to color function to manipulate every vertex's color
    Graph3D(const SurfaceDescription &L, const ColorMode CM, const GLuint ProgID = DefaultProgram(),
            const Grid *grid_ref = NULL, const glm::vec4 (*ColorFunc2D)(const glm::vec2 &) = NULL);

    /// @brief Update the surface
    /// @param NewSurface The surface
    void UpdateFunction(const SurfaceDescription &NewSurface);
    // void Draw2(float linewidth);
    void Draw() override;
};

/// @brief Smooth change from curve to another
class FunctionSweep {
  private:
  public:
    const LineFunction *LF;
    const Graph3D *G3D;
    GPUBuffer B1;
    ComputeShader ComputeSweep;

    /// @brief Initialize curve sweep
    /// @param Line A pointer to line class
    /// @param L1 Description of the first line
    /// @param L2 Description of the seconds line
    FunctionSweep(LineFunction *Line, const LineFunction::LineDescription &L1, const LineFunction::LineDescription &L2);

    /// @brief Initialize surface sweep
    /// @param g3d A pointer to Graph3D class
    /// @param S1 Description of the first curve
    /// @param S2 Description of the second curve
    FunctionSweep(Graph3D *g3d, const Graph3D::SurfaceDescription &S1, const Graph3D::SurfaceDescription &S2);
    void lerp(float t);
};


















class ChartFunction : public Program {

  public:
    /*    struct LineDescription {
           R1R3 function;
           float a;
           float b;
           int steps;
           LineDescription() {
           }
           /// @brief Discribe path function using intervals, number of steps and a function
           /// @param f A pointer of path function (float -> glm::vec3)
           /// @param t1 The first value in the interval
           /// @param t2 The last value in the interval
           /// @param _steps Number of steps
           LineDescription(R1R3 f, float t1, float t2, int _steps) : function(f), a(t1), b(t2), steps(_steps) {
           }
       }; */
    // struct SubFunction {
    //     int offset;
    //     int length;
    //     glm::vec3 spatial_shift;

    //     /// @brief Sub function description
    //     /// @param _offset buffer offset (measured in the # of elements to skip)
    //     /// @param _vertices_count Number of vertices to draw
    //     /// @param _spatial_shift Translation vector
    //     SubFunction() : offset(0), length(0), spatial_shift({}) {
    //     }
    //     SubFunction(const int _offset, const int _vertices_count, const glm::vec3 &_spatial_shift = {})
    //         : offset(_offset), length(_vertices_count), spatial_shift(_spatial_shift) {
    //     }
    // };

  private:
    using LineDescription = Cycloid::LineFunction::LineDescription;

    ArrayBuffer AB;
    LineDescription mLine;
    float LineWidth;
    ColorMode mCM;
    const bool onGrid;
    glm::vec3 coords[3];

    unsigned int nVertices;
    const unsigned int nIndices;
    void *vertices;
    unsigned int *indices;
    bool MutiFun = false;

    std::vector<int> MultipleFunctions;

    /// @brief Set Colour mode
    /// @param CM Color mode (Solid, Per-vertex, Per-Position)
    /// @param solid Solid color if the choosen mode is solid
    void SetMode(const ColorMode CM, const glm::vec4 &solid = {0.1, 0.3, 0.23, 1});
    // solid color mode

    /// @brief Init solid colored line
    void SMode();
    // Not Tested

    /// @brief Init Per-vertex curve
    /// @param ColorFunc A pointer to color function (float -> RGBA)
    void PVMode(const glm::vec4 (*ColorFunc)(float));

    /// @brief Change current function
    /// @param LD Line description for the new function
    void ChangeFunction(const LineDescription &LD);

  public:
    friend class FunctionSweep;

    static GLuint DefaultProgram() {
        return GL::GetProgram("../shaders/Cycloid/vertex.glsl", "../shaders/Cycloid/f_function.glsl", NULL);
    }
    ~ChartFunction() {
        if (vertices != NULL)
            delete[] vertices;
        //    if (cvertices != NULL)delete[] cvertices;
    }
    /// @brief Initialize path function
    /// @param L Line description
    /// @param CM Color Mode
    /// @param ProgID Shader Program ID
    /// @param grid_ref If the function is relative to grid (null for default)
    /// @param ColorFunc Color function (Set for CM_PERVERTEX color mode)
    ChartFunction(const LineDescription &L, const ColorMode CM,
                  const GLuint ProgID = DefaultProgram(), const Grid *grid_ref = NULL,
                  const glm::vec4 (*ColorFunc)(float) = NULL);

    /// @brief Update the current function
    /// @param NewLine Decription for the new line
    void UpdateFunction(const LineDescription &NewLine);

    /// @brief Update GPU buffer (copy CPU buffer)
    void UpdateVertices();

    /// @brief Draw the function
    /// @param linewidth The width in pixels
    /// @param points Number of verices to draw
    void Draw2(float linewidth, const int points = -1);

    /// @brief Default vanella draw call
    void Draw() override;

    // void MultipleFunctions(const std::vector<int> &offsets) {
    //     int acc = 0;
    //     for (int i = 0; i < offsets.size(); i++) {
    //         acc += offsets[i];
    //     }
    // }
    // void DrawMultipleFunctions(float linewidth = 1.5) {
    //     if (MutiFun) {

    //         glLineWidth(linewidth);
    //         glUseProgram(ProgramID);
    //         glBindVertexArray(SceneData->VAO);
    //         int acc = 0;
    //         for (int i = 0; i < MultipleFunctions.size(); i++) {
    //             const auto k = MultipleFunctions[i];
    //             glBindBuffer(GL_ARRAY_BUFFER, SceneData->GBuf.buf_idx);
    //             // std::cout << ((points > -1) ? points : mLine.steps) << std::endl;
    //             glDrawArrays(GL_LINE_STRIP, acc, k);
    //             acc += k;
    //         }
    //     }
    // }

    // void DrawMulFunc(const std::vector<SubFunction> &sub_functions, const float linewidth = 1.5) {

    //     const int n_functions = sub_functions.size();

    //     glLineWidth(linewidth);
    //     glUseProgram(ProgramID);
    //     glBindVertexArray(SceneData->VAO);

    //     for (int i = 0; i < n_functions; ++i) {
    //         glUniform3fv(0, 1, glm::value_ptr(sub_functions[i].spatial_shift));
    //         glDrawArrays(GL_LINE_STRIP, sub_functions[i].offset, sub_functions[i].length);
    //     }
    // }

    GPUBuffer GetVBO() const {
        return AB.GBuf;
    }

    /// @brief Append new values to the function (Shift old then add new)
    /// @note SLOW
    /// @param new_vals A pointer to a buffer contains vertices
    /// @param count The number of vertices to append
    void AppendValue(const glm::vec3 *new_vals, const int count) {
        // const int stride = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);
        if (mCM == CM_PERVERTEX) {
            ColoredVertex *ptr = (ColoredVertex *)vertices;
            for (int i = 0; i < mLine.steps - count; i++) {
                ptr[i].vertex = ptr[i + count].vertex;
            }
            const int offset = mLine.steps - count;
            for (int i = 0; i < count; i++) {
                *(glm::vec3 *)&ptr[i + offset] = new_vals[i];
            }
            SceneData->UpdateBuffer(0, mLine.steps * sizeof(ColoredVertex), vertices);
        } else {
            glm::vec4 *ptr = (glm::vec4 *)vertices;
            for (int i = 0; i < mLine.steps - count; i++) {
                ptr[i] = ptr[i + count];
            }
            const int offset = mLine.steps - count;
            for (int i = 0; i < count; i++) {
                *(glm::vec3 *)&ptr[i + offset] = new_vals[i];
            }
            SceneData->UpdateBuffer(0, mLine.steps * sizeof(glm::vec4), vertices);
        }
    }

    /// @brief Append new values to the function (Shift old then add new) using memory mapping
    /// @note FASTER than AppendValue()
    /// @param new_vals A pointer to a buffer contains vertices
    /// @param count The number of vertices to append
    void AppendValue2(const glm::vec4 *new_vals, const int count) {
        const int stride = (mCM == CM_PERVERTEX) ? sizeof(ColoredVertex) : sizeof(glm::vec4);
        char *map = (char *)glMapNamedBuffer(SceneData->GBuf.buf_idx, GL_READ_WRITE);
        if (count < mLine.steps)
            memmove(map, map + stride * count, (this->mLine.steps - count) * stride);
        memcpy(map + stride * (this->mLine.steps - count), new_vals, count * stride);
        glUnmapNamedBuffer(SceneData->GBuf.buf_idx);
    }

    /// @brief Get a pointer to CPU buffer
    /// @return return a void pointer
    void *GetVertices() const {
        return vertices;
    }
};






}; // namespace Cycloid

#endif
