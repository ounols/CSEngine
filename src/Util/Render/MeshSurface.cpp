#include "MeshSurface.h"
#include "../../Manager/MemoryMgr.h"
#include "../../OGLDef.h"
#include "../MoreString.h"
#include "../../Object/SPrefab.h"
#include "../../Manager/EngineCore.h"
// #include <iostream>

using namespace CSE;

MeshSurface::MeshSurface() = default;

MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals) : m_faceSize(0), m_vertexSize(0),
                                                                          m_indexSize(-1) {
    MakeVertices(sizeVert, vertices, normals, nullptr, nullptr, nullptr);
}


MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals, float* texCoords) : m_faceSize(0),
                                                                                            m_vertexSize(0),
                                                                                            m_indexSize(-1) {
    MakeVertices(sizeVert, vertices, normals, texCoords, nullptr, nullptr);
}


MeshSurface::~MeshSurface() = default;

bool MeshSurface::MakeVertices(int sizeVert, float* vertices, float* normals, float* texCoords, float* weights,
                               short* jointIds) {
    if (!m_Verts.empty()) return false;

    struct Vertex {
        vec3 Position;
        vec3 Normal;
        vec2 TexCoord;
        vec3 Weight;
        vec3 JointId;
    };

    if(jointIds != nullptr) m_meshId.m_hasJoint = true;
    m_Verts.resize(sizeVert * 14);

    auto vertex_tmp = reinterpret_cast<Vertex*>(&m_Verts[0]);

    for (int i = 0; i < sizeVert; ++i) {
        vertex_tmp->Position.x = *(vertices)++;
        vertex_tmp->Position.y = *(vertices)++;
        vertex_tmp->Position.z = *(vertices)++;

        const auto d = vec3::Distance(vec3::Zero, vertex_tmp->Position);
        if(d > m_meshId.m_maxSize)
            m_meshId.m_maxSize = d;

        if (normals == nullptr) {
            vertex_tmp->Normal.Set(0, 0, 0);
        } else {
            vertex_tmp->Normal.x = *(normals)++;
            vertex_tmp->Normal.y = *(normals)++;
            vertex_tmp->Normal.z = *(normals)++;
        }

        if (texCoords == nullptr) {
            vertex_tmp->TexCoord.x = 0;
            vertex_tmp->TexCoord.y = 0;
        } else {
            vertex_tmp->TexCoord.x = *(texCoords)++;
            vertex_tmp->TexCoord.y = *(texCoords)++;
        }

        if (weights == nullptr) {
            vertex_tmp->Weight.Set(0, 0, 0);
        } else {
            vertex_tmp->Weight.x = *(weights)++;
            vertex_tmp->Weight.y = *(weights)++;
            vertex_tmp->Weight.z = *(weights)++;
        }

        if (jointIds == nullptr) {
            vertex_tmp->JointId.Set(0, 0, 0);
        } else {
            vertex_tmp->JointId.x = *(jointIds)++;
            vertex_tmp->JointId.y = *(jointIds)++;
            vertex_tmp->JointId.z = *(jointIds)++;
        }
        vertex_tmp++;
    }
    m_vertexSize = sizeVert;
    return true;
}


bool MeshSurface::MakeIndices(int sizeIndic, int* indices) {
    if (!m_Indics.empty()) return false;

    m_Indics.resize(sizeIndic * 3);

    for (int i = 0; i < sizeIndic; ++i) {
        m_Indics[i * 3] = static_cast<unsigned short>(*(indices)++);
        m_Indics[i * 3 + 1] = static_cast<unsigned short>(*(indices)++);
        m_Indics[i * 3 + 2] = static_cast<unsigned short>(*(indices)++);
    }

    m_indexSize = sizeIndic;
    return true;
}

int MeshSurface::GetVertexCount() const {
    return m_vertexSize;
}

int MeshSurface::GetLineIndexCount() const {
    return -1;
}

int MeshSurface::GetTriangleIndexCount() const {
    return m_indexSize;
}

void MeshSurface::GenerateVertices(std::vector<float>& vertices, unsigned char flags) const {
    vertices.resize(GetVertexCount() * 14); // xzy + xyz + st
    vertices = m_Verts;

}

void MeshSurface::GenerateLineIndices(std::vector<unsigned short>& indices) const {
    indices.resize(GetTriangleIndexCount() * 3); //xyz
    indices = m_Indics;
}

void MeshSurface::GenerateTriangleIndices(std::vector<unsigned short>& indices) const {
    indices.resize(GetTriangleIndexCount() * 3);
    indices = m_Indics;
}

bool MeshSurface::HasJoint() const {
    return m_meshId.m_hasJoint;
}

vec3 MeshSurface::GenerateTopTriangle(const vec3& v0, const vec3& v1, const vec3& v2) {
    float height = v1.y - v0.y;
    float width = 0.0f;
    vec3 S;
    vec3 E;
    vec3 N;

    for (int i = 0; i < (int)height; ++i) {
        float kCoff = (float)i / height;

        S = LerpFilter(v0, v1, kCoff);
        E = LerpFilter(v0, v2, kCoff);
        N = S.Cross(E).Normalized();
    }
    return N;
}


vec3 MeshSurface::GenerateBottomTriangle(const vec3& v0, const vec3& v1, const vec3& v2) {
    float height = v2.y - v0.y;
    float width = 0.0f;
    vec3 S;
    vec3 E;
    vec3 N;

    for (int i = 0; i < (int)height; ++i) {
        float kCoff = (float)i / height;

        S = LerpFilter(v0, v2, kCoff);
        E = LerpFilter(v1, v2, kCoff);
        N = S.Cross(E).Normalized();
    }
    return N;
}


vec3 MeshSurface::LerpFilter(const vec3& v0, const vec3& v1, float kCoff) {
    vec3 v = v1 * kCoff + (v0 * (1.0f - kCoff));
    return v;
}


void MeshSurface::Exterminate() {
    const GLuint vertexBuffer = m_meshId.m_vertexBuffer;
    const GLuint indexBuffer = m_meshId.m_indexBuffer;

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}


void MeshSurface::Destroy() {
    CORE->GetCore(MemoryMgr)->ReleaseObject(this);
}

void MeshSurface::Init(const AssetMgr::AssetReference* asset) {
    std::string parent_id = split(asset->id, '?')[0];
    auto model = CORE->GetCore(ResMgr)->GetAssetReference(parent_id);
    AssetMgr::TYPE type = model->type;

    // 프리팹에 모든 정보가 있으므로 아예 프리팹 새로 생성
    // 프리팹 객체에서만 모든걸 만들어야 로직이 꼬이지 않기 때문에 해당 예외처리는 허용되지 않음 (삭제 예정)
//    SPrefab* prefab = nullptr;
//
//    switch (type) {
//        case AssetMgr::DAE:
//            prefab = DAELoader::GeneratePrefab(model->path.c_str(), nullptr, this, nullptr, nullptr);
//            break;
//        default:
//            break;
//    }
}