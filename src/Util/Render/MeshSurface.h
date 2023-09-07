#pragma once

#include "RenderInterfaces.h"
#include "../../Object/SResource.h"
#include "../Vector.h"

namespace CSE {

    class MeshSurface : public SISurface {
    public:
        MeshSurface();
        MeshSurface(int sizeVert, float* vertices, float* normals);
        MeshSurface(int sizeVert, float* vertices, float* normals, float* texCoords);
//        MeshSurface(int sizeVert, int sizeIndic, float* vertices, float* normals, float* texCoords, float* indices);
        ~MeshSurface() override;

        int GetVertexCount() const override;
        int GetLineIndexCount() const override;
        int GetTriangleIndexCount() const override;
        void GenerateVertices(std::vector<float>& vertices, unsigned char flags) const override;
        void GenerateLineIndices(std::vector<unsigned short>& indices) const override;
        void GenerateTriangleIndices(std::vector<unsigned short>& indices) const override;

        bool HasJoint() const;

        static vec3 GenerateTopTriangle(const vec3& v0, const vec3& v1, const vec3& v2);
        static vec3 GenerateBottomTriangle(const vec3& v0, const vec3& v1, const vec3& v2);
        static vec3 LerpFilter(const vec3& v0, const vec3& v1, float kCoff);

        void Exterminate() override;
        void Destroy() override;

        bool
        MakeVertices(int sizeVert, float* vertices, float* normals, float* texCoords, float* weights, short* jointIds);
        bool MakeIndices(int sizeIndic, int* indices);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        mutable size_t m_faceSize = 0;
        mutable size_t m_vertexSize = 0;
        mutable size_t m_indexSize = 0;

        std::vector<float> m_Verts;
        std::vector<unsigned short> m_Indics;
    };
}