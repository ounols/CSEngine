#pragma once
#include "RenderInterfaces.h"
#include "../../Object/SResource.h"
#include "../Vector.h"

class MeshSurface : public SISurface {
public:
	MeshSurface();
	MeshSurface(int sizeVert, float* vertices, float* normals);
	MeshSurface(int sizeVert, float* vertices, float* normals, float* texCoords);
//	MeshSurface(int sizeVert, int sizeIndic, float* vertices, float* normals, float* texCoords, float* indices);
	~MeshSurface() override;

	int GetVertexCount() const override;
	int GetLineIndexCount() const override;
	int GetTriangleIndexCount() const override;
	void GenerateVertices(std::vector<float>& vertices, unsigned char flags) const override;
	void GenerateLineIndices(std::vector<unsigned short>& indices) const override;
	void GenerateTriangleIndices(std::vector<unsigned short>& indices) const override;


    static vec3 GenerateTopTriangle(vec3 v0, vec3 v1, vec3 v2);
	static vec3 GenerateBottomTriangle(vec3 v0, vec3 v1, vec3 v2);
	static vec3 LerpFilter(vec3 v0, vec3 v1, float kCoff);


	void Exterminate() override;
	void Destroy();


	bool MakeVertices(int sizeVert, float* vertices, float* normals, float* texCoords, float* weights, float* jointIds);
	bool MakeIndices(int sizeIndic, int* indices);

protected:
    void Init(const AssetMgr::AssetReference* asset) override;

private:
	mutable size_t m_faceSize;
	mutable size_t m_vertexSize;
	mutable size_t m_indexSize;

	std::vector<float> m_Verts;
	std::vector<unsigned short> m_Indics;

};

