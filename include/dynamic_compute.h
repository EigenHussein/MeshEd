#ifndef DYNCOMPSHADER_HH
#define DYNCOMPSHADER_HH

const char p1[] = R"(
  #version 460 core
uniform float radius_cs = 1.f;
uint nElemets = 513 + 1;//wire element
uint nCE = 129 + 1; //cross section elements
const vec2 start_end = vec2(-1, 1);
float dt=(start_end[1]-start_end[0])/float(nElemets-1);
struct MVertex {
    vec4 Position;
    vec4 Normal;
};

layout(std430, binding = 2) buffer Cir{
    vec4 Circle[ /* nCE */];
};

layout(std430, binding = 3) buffer Wire {
    MVertex WireElements[/*  (nCE)*2*(nElemets)  */];
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

vec3 wire_path(float t);
mat3 GetTransformation(vec3 p0, vec3 p1) {

    vec3 w = normalize(p0 - p1);
    vec3 u = cross(vec3(0.f, 0.0f, 1.f), w);
    vec3 v = cross(w, u);//the amount of insecurity in this line... holy god
    return mat3(u, v, w);

}
void Fill(uint idx){

    float t = start_end[0] + dt * idx;
    vec3 p1= wire_path(t);
    vec3 p2= wire_path(t + dt);
    mat3 RotMat = GetTransformation(p1, p2);
    for (int j = 0; j < nCE; j++) {
        WireElements[j + idx.x * nCE].Position.xyz = p1 + RotMat * radius_cs*Circle[j].xyz;
    }
    
}
// void Fill(uvec2 idx){
//     float t=start_end[0]+dt*idx.x;
//     vec3 p1=wire_path(t);
//     vec3 p2=wire_path(t+dt);
//     mat3 RotMat = GetTransformation(p1, p2);
//     WireElements[idx.x*2+0].Position.xyz=p1;
//     WireElements[idx.x*2+1].Position.xyz=p1+RotMat*vec3(1,0,0);
// }
void main() {

    uint id = gl_GlobalInvocationID.x;
    Fill(id);
}

vec3 wire_path(float t) {
    return 
)";





const char P0x00A_Dyn[] = R"(#version 460 core
#define pi 3.1415926535

struct VectorField {
    vec4 Position;
    vec4 Direction;
};

struct ModelMatrices {
    mat4 Rotation;
    mat4 Translation;
    vec4 Position;
    vec4 Direction;
    vec4 Color;

};


const uint FieldSize = 64;
const uint InterSize = 16;
#define DIFF ((FieldSize-InterSize)/2)


layout(std430, binding = 0) buffer BF_ {
    //ModelMatrices Info[InterSize][InterSize][InterSize];
    ModelMatrices Info[];//[InterSize*InterSize*InterSize];
};

layout(std430, binding = 1) buffer VF_ {
    //VectorField VF[FieldSize][FieldSize][FieldSize];
    VectorField VF[];//[FieldSize*FieldSize*FieldSize];
};



mat4 Rotate(vec3 ref) {
    mat4 tmp;
    tmp[3] = vec4(0, 0, 0, 1);
    tmp[2] = vec4(ref, 0);
    tmp[1] = vec4(normalize(cross(vec3(0.7071, 0, 0.7071), ref)), 0);
    tmp[0] = vec4(normalize(cross(cross(vec3(0.7071, 0, 0.7071), ref), ref)), 0);
    return tmp;
}
mat4 Translate(vec3 ref) {
    mat4 tmp;
    tmp[3] = vec4(ref, 1);
    tmp[2] = vec4(0, 0, 1, 0);
    tmp[1] = vec4(0, 1, 0, 0);
    tmp[0] = vec4(1, 0, 0, 0);
    return tmp;
}

vec3 path(float t);
vec3 Magnet(vec3 p) {
    vec3 acc = vec3(0);
    const float dt = 0.001;
    for(float i = -1;
    i < 1.f;
    i += 0.001) {
        vec3 point_ = path(i);
        float R = length(p - point_);
        vec3 L = normalize((path(i + dt) - point_) / dt);
        vec3 D = normalize(p - point_);
        acc = acc + cross(L, D) / pow(R, 2) * dt*4;//*****************4*/
    }
return acc;
}
vec3 Field(vec3 p) {
    return normalize(vec3(- p.y, p.x, 0));
}

#define LERP(A,B,t) (A+(B-A)*t)

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

const vec3 steps_vf = vec3(0.375);
const vec3 head_vf=vec3(-12,-12,-12);
const vec3 steps = vec3(1);
const vec3 head = vec3(- 8, - 8, - 8);
    
layout(local_size_x = 2, local_size_y = 2, local_size_z = 2) in;

void main() {

    uvec3 id = gl_GlobalInvocationID;
    vec3 location = head_vf + steps_vf * id;
    vec3 Value = Magnet(location);//Magnet(location);
    //float VectorLength = length(Value);
        //vec3 RotVector = normalize();
    uint index = id.x * FieldSize * FieldSize + id.y * FieldSize + id.z;
    VF[index].Position = vec4(location,0);//vec4(id.x,id.y,id.z, 0);
    VF[index].Direction = vec4(Value, 0);//vec4(Value/VectorLength,VectorLength);

    // if(
    //     /* (id.x >= 4 && id.x<20)&&
    //     (id.y >= 4 && id.y<20)&&
    //     (id.z >= 4 && id.z<20) */
    //     (id.x < 16)&&
    //     (id.y < 16)&&
    //     (id.z < 16)
    // ) {
    //     vec3 L = head + steps * id;
    //     vec3 V = Magnet(L);
    //     Info[index].Position = vec4(id.x, id.y, id.z);//vec4(L, 0);
    //     Info[index].Direction = vec4(V, 0);
    // }

}

vec3 path(float t) {
    //t = 2*pi*t;
    return 
)";




// char wire_function[256];
// bool wire_function_update = 0;
// bool radius_slider_update = 0;float radius_cs = 1.f;

// void gui_wire_function() {
//     GUI::StartImGui();
//     ImGui::Begin("Wire function update");
//     bool text_enter = ImGui::InputText("Line Function:", wire_function, sizeof(wire_function), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
//     ImGui::SameLine();
//     bool button_click = ImGui::Button("Enter");
//     radius_slider_update = ImGui::SliderFloat("cross section radius", &radius_cs, 0.01, 5);
//     wire_function_update = text_enter | button_click;
//     GUI::EndImGui();
// }

char wire_function[256];
bool wire_function_update = 0;
bool radius_slider_update = 0;float radius_cs = 1.f;
bool reset_request = false;
void gui_wire_function() {
    GUI::StartImGui();
    ImGui::Begin("Wire function update");
    bool text_enter = ImGui::InputText("Line Function:", wire_function, sizeof(wire_function), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    bool button_click = ImGui::Button("Enter");
    radius_slider_update = ImGui::SliderFloat("cross section radius", &radius_cs, 0.01, 5);
    wire_function_update = text_enter | button_click;
    ImGui::NewLine();
    reset_request = ImGui::Button("Reset");
    GUI::EndImGui();
}

GLuint new_compute(ComputeShader* cshader, const char* func) {
    std::string csource = p1;
    csource += func + std::string(";\n}");
    if (cshader != NULL) {
        return cshader->UpdateProgram(csource.c_str());
    }

}
class WireRenderer {
private:
public:
    ComputeShader Wire;
    ComputeShader NormalGenerator;
    iMesh* WireMesh;
    Camera* TopCamera;
    int nPS;
    int nCS;
    void UpdateWire() {
        glUseProgram(Wire.ProgramID);
        glUniform1f(glGetUniformLocation(Wire.ProgramID, "radius_cs"), radius_cs);
        glDispatchCompute(nPS - 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    };
    void UpdateNormals() {
        glUseProgram(NormalGenerator.ProgramID);
        glDispatchCompute(nPS - 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    };

    GLuint getProgramID()const {
        return WireMesh->ProgramID;
    }
    WireRenderer(Camera& Cam, const char* init_wire = "vec3(cos(5*t),sin(5*t),t)", int ncs = 130, int nps = 514) :

        Wire(ComputeShader("../shaders/0x009/wire_generator.glsl")),
        NormalGenerator(ComputeShader("../shaders/0x009/norm_generator.glsl")), nPS(nps), nCS(ncs),
        TopCamera(&Cam), WireMesh(NULL)
    {
        new_compute(&Wire, "vec3(cos(5*t),sin(5*t),t)");

        glm::vec4* cir = new glm::vec4[nCS];

        for (int i = 0;i < nCS;i++) {
            float t = i * 2 * M_PI / float(nCS - 1);
            cir[i] = { cos(t),sin(t),0,0 };
        }
        //circle
        auto CircleSSBO = Wire.AddSSBO(sizeof(glm::vec4) * (nCS), 2);
        Wire.SetData(0, cir, sizeof(glm::vec4) * (nCS), 0);
        //wire
        auto SSBO = Wire.AddSSBO(nPS * nCS * 2 * sizeof(glm::vec4), 3);
        auto indices = MeshGenerator::CPUMeshGenerator::gen_inds(nPS, nCS);

        GLuint Prog = GL::GetProgram("../shaders/0x009/v.glsl", "../shaders/0x009/f.glsl", NULL);

        WireMesh = new iMesh(DT_ELEMENT, SSBO, { GLAttrib(0,3,8,0),GLAttrib(1,3,8,4) }, nCS * nPS, indices.Indices,
            indices.nIndices - 6 * (nCS - 1), GL_TRIANGLES, Prog);
        Cam.AttachProgram(*WireMesh);
        Cam.Update(CU_PROJ_VIEW);
        //glPointSize(3);

        free(indices.Indices);
        delete[] cir;
        //std::cout << "Ended Class" << std::endl;
        UpdateWire();
        UpdateNormals();
        // VectorField VF[1000];

        // Wire.GetData(1, VF, sizeof(VF), 0);
        // for (int i = 0;i < 1000;i++) {
        //     std::cout << i << '\t' << VF[i].Position.x << '\t' << VF[i].Position.y << '\t' << VF[i].Position.z << "\n";
        // }


    }
    void draw() {
        //gui_wire_function();
        if (wire_function_update) {
            new_compute(&Wire, wire_function);
            UpdateWire();
            UpdateNormals();
        }
        else if (radius_slider_update) {
            glUseProgram(Wire.ProgramID);
            glUniform1f(glGetUniformLocation(Wire.ProgramID, "radius_cs"), radius_cs);
            UpdateWire();
            UpdateNormals();
        }
        WireMesh->Draw();
        //WireMesh->Draw2(18);
        //WireMesh->Draw2(10000,3);
    }
    ~WireRenderer() {
        delete WireMesh;
    }
};


#endif
