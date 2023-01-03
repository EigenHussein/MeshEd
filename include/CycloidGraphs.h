#ifndef CYCLOID_GRAPHS_HH
#define CYCLOID_GRAPHS_HH
#include <Core2.h>
#include <Program.h>
#include <glm/glm.hpp>
#include <vector>
namespace Cycloid {

    namespace VertexAttributes {
        // const std::vector<GLAttrib> ATTRIB_V = { GLAttrib(0,3,3,0) };
        // const std::vector<GLAttrib> ATTRIB_VN = { GLAttrib(0,3,6,0),GLAttrib(1,3,6,3) };
        // const std::vector<GLAttrib> ATTRIB_VC3 = ATTRIB_VN;
        // const std::vector<GLAttrib> ATTRIB_
        enum Attribs {
            VA_NULL = 0,
            VA_VERTEX = 1,
            VA_NORMAL = 2,
            VA_COLOR = 4,
            VA_UV = 8,
            VA_VN = VA_VERTEX | VA_NORMAL,
            VA_VC = VA_VERTEX | VA_COLOR,
            VA_VNC = VA_VERTEX | VA_COLOR | VA_NORMAL
        };
    }

    typedef     float(*R1R1)(float);
    typedef glm::vec2(*R1R2)(float);
    typedef glm::vec3(*R1R3)(float);
    typedef glm::vec3(*R2R3)(glm::vec2);


    enum ObjectType {
        CYC_BACKGROUND = GL_TRIANGLES,
        CYC_LINES = GL_LINES,
        CYC_2D_FUNCTION = GL_LINE_STRIP,
        CYC_3D_FUNCTION = GL_TRIANGLES
    };
    struct Plane {
        glm::vec3 origin;
        glm::vec3 uAxis;
        glm::vec3 vAxis;
        Plane(const glm::vec3& o, const glm::vec3& u, const glm::vec3& v) :origin(o), uAxis(u), vAxis(v) {}
    };
    class ObjectDescription {
    private:
        int AttribSize(VertexAttributes::Attribs VA) {
            int sz =
                ((VA & VertexAttributes::VA_VERTEX) > 0) * 3 +
                ((VA & VertexAttributes::VA_NORMAL) > 0) * 3 +
                ((VA & VertexAttributes::VA_COLOR) > 0) * 4 +
                ((VA & VertexAttributes::VA_UV) > 0) * 2;
            return sz;
        }
        void CreateVAO(VertexAttributes::Attribs VA, int ObjectOffset) {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            int offset = 0;
            int sz = AttribSize(VA);
            if (VA & VertexAttributes::VA_VERTEX) {
                GLAttrib(0, 3, sz, ObjectOffset + offset).SetAttrib();
                offset += 3;
            }

            if (VA & VertexAttributes::VA_NORMAL) {
                GLAttrib(1, 3, sz, ObjectOffset + offset).SetAttrib();
                offset += 3;
            }

            if (VA & VertexAttributes::VA_COLOR) {
                GLAttrib(2, 4, sz, ObjectOffset + offset).SetAttrib();
                offset += 4;
            }
            if (VA & VertexAttributes::VA_UV) {
                GLAttrib(3, 2, sz, ObjectOffset + offset).SetAttrib();
            }
            glBindVertexArray(0);
        }
    public:
        ObjectType Type;
        int nFloats;
        //Attributes
        GLuint VAO;
        int nIndices;
        int offset;
        ObjectDescription(ObjectType _Type, int nF, int nI, int float_offset, VertexAttributes::Attribs VA = VertexAttributes::VA_VERTEX, void(*CustomAttrib)() = NULL)
            :Type(_Type), nFloats(nF), nIndices(nI), offset(float_offset) {
            if (VA != VertexAttributes::VA_NULL) {
                CreateVAO(VA, offset);
            }
            else {
                assert(CustomAttrib != NULL);
                CustomAttrib();
            }
        }
    };


    const Plane XYPlane = { {0,0,0},{1,0,0},{0,1,0} };

    class Grapher2D : public Program {

    private:
        ArrayBuffer AB;
        GLuint BackgoundShader;
    public:
        struct ColoredVertex {
            glm::vec3 Position;
            glm::vec4 Color;
            ColoredVertex(const glm::vec3& p, const glm::vec4& c) :Position(p), Color(c) {}

        };
        std::vector<ObjectDescription> Objects;
        std::vector<ColoredVertex> Vertices;
        std::vector<unsigned int> Indices;

        ObjectDescription NewObjectDesc(ObjectType OT, int Obj_nVertices, int Obj_nIndices) {
            return{
                OT,
                Obj_nVertices,
                Obj_nIndices,
                (Objects.size() == 0) ? 0 : (Objects.back().offset + Objects.back().nIndices)
            };
        }

        //public:
        void Draw() override {
            glUseProgram(ProgramID);
            glBindVertexArray(SceneData->VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SceneData->EBO);
            for (auto Obj : Objects) {
                glDrawElementsBaseVertex(Obj.Type, Obj.nIndices, GL_UNSIGNED_INT, 0, Obj.offset);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        Grapher2D(int nObjects, GLuint ProgID = DefualtProgram(), int nVertices = 1000, int nIndices = 1000) :Program(), AB(DT_ELEMENT) {
            ProgramID = (ProgID == -1) ? DefualtProgram() : ProgID;
            Objects.reserve(nObjects);
            Vertices.reserve(nIndices);
            Vertices.reserve(nIndices);
        }

        ObjectDescription& AddGrid(
            const glm::ivec2& blocks,
            const glm::ivec2& subblocks,
            const glm::vec2& seperation,
            const glm::vec4& main_color = { 0,0,0,1 },
            const glm::vec4& sub_color = { 0.4,0.4,0.4,1 },
            const glm::vec3& origin = { 0,0,0 },
            const glm::vec3& uGrid = { 1,0,0 },
            const glm::vec3& vGrid = { 0,1,0 }
        );


        ObjectDescription& Add2DFunction(R1R1 function, float a, float b, int nSteps, const Plane& Plane = XYPlane);

        void Finish() {
            // this->number_of_vertices = nVertices;
            // this->number_of_indices = nIndices;
            // this->draw_t = Dtype;
            // ProgramID = _ProgramID;
            // mode = DrawMode;
            std::vector<DataBuffer> bufs;
            // for (auto Obj:Objects) {
            //     bufs.emplace_back(Vertices.data()[Obj.])
            // }
            AB.AllocateBuffer(
                AttributedData(
                    {
                        DataBuffer(Vertices.data(),Vertices.size() * sizeof(ColoredVertex)),
                    },
                   {
                       GLAttrib(0,3,7,0),GLAttrib(2,4,7,3)
                   }
                )
            );

            AB.AllocateElements(Indices.data(), Indices.size() * sizeof(unsigned int));

            this->AttachBuffer(AB);
        }

        
        static GLuint DefualtProgram() {
            return GL::GetProgram("../shaders/Cycloid/vertex.glsl", "../shaders/Cycloid/fragment.glsl", NULL);
        }
    };

};



#endif
