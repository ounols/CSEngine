#include "MeshSurface.h"
#include "../../Manager/MemoryMgr.h"
#include "../../OGLDef.h"
#include "../MoreString.h"
#include "../../Object/SPrefab.h"
#include "../Loader/DAE/DAELoader.h"
#include "../../Manager/EngineCore.h"
// #include <iostream>

using namespace CSE;

MeshSurface::MeshSurface() {
}

MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals) : m_faceSize(0), m_vertexSize(0),
                                                                          m_indexSize(-1) {

    MakeVertices(sizeVert, vertices, normals, nullptr, nullptr, nullptr);
}


MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals, float* texCoords) : m_faceSize(0),
                                                                                            m_vertexSize(0),
                                                                                            m_indexSize(-1) {

    MakeVertices(sizeVert, vertices, normals, texCoords, nullptr, nullptr);

}


MeshSurface::~MeshSurface() {

}

bool MeshSurface::MakeVertices(int sizeVert, float* vertices, float* normals, float* texCoords, float* weights,
                               float* jointIds) {
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

    Vertex* vertex_tmp = reinterpret_cast<Vertex*>(&m_Verts[0]);

    for (int i = 0; i < sizeVert; ++i) {
        vertex_tmp->Position.x = *(vertices)++;
        vertex_tmp->Position.y = *(vertices)++;
        vertex_tmp->Position.z = *(vertices)++;

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

    struct Index {
        Vector3<unsigned short> index;
    };

    m_Indics.resize(sizeIndic * 3);

    Index* index_tmp = reinterpret_cast<Index*>(&m_Indics[0]);

    for (int i = 0; i < sizeIndic; ++i) {
        index_tmp->index.x = static_cast<unsigned short>(*(indices)++);
        index_tmp->index.y = static_cast<unsigned short>(*(indices)++);
        index_tmp->index.z = static_cast<unsigned short>(*(indices)++);

        index_tmp++;
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


vec3 MeshSurface::GenerateTopTriangle(vec3 v0, vec3 v1, vec3 v2) {

    float height = v1.y - v0.y;
    float width = 0.0f;

    vec3 S;
    vec3 E;
    vec3 N;

    for (int i = 0; i < height; ++i) {
        float kCoff = i / height;

        S = LerpFilter(v0, v1, kCoff);
        E = LerpFilter(v0, v2, kCoff);

        N = S.Cross(E).Normalized();
    }

    return N;

}


vec3 MeshSurface::GenerateBottomTriangle(vec3 v0, vec3 v1, vec3 v2) {

    float height = v2.y - v0.y;
    float width = 0.0f;

    vec3 S;
    vec3 E;
    vec3 N;

    for (int i = 0; i < height; ++i) {
        float kCoff = i / height;

        S = LerpFilter(v0, v2, kCoff);
        E = LerpFilter(v1, v2, kCoff);

        N = S.Cross(E).Normalized();
    }

    return N;

}


vec3 MeshSurface::LerpFilter(vec3 v0, vec3 v1, float kCoff) {

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
    CORE->GetCore<MemoryMgr>()->ReleaseObject(this);
}

void MeshSurface::Init(const AssetMgr::AssetReference* asset) {

    std::string parent_id = split(asset->id, '?')[0];
    auto model = CORE->GetCore<ResMgr>()->GetAssetReference(parent_id);
    AssetMgr::TYPE type = model->type;

    //프리팹에 모든 정보가 있으므로 아예 프리팹 새로 생성
    SPrefab* prefab = nullptr;

    switch (type) {
        case AssetMgr::DAE:
            prefab = DAELoader::GeneratePrefab(model->path.c_str(), nullptr, this, nullptr, nullptr);
            break;
        default:
            break;
    }

    return;
}