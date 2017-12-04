#pragma once
#include "RenderInterfaces.h"
#include "../Vector.h"

class ObjSurface : public SISurface {
public:
	ObjSurface(int sizeVert, double* vertices, double* normals);
	ObjSurface(int sizeVert, double* vertices, double* normals, double* texCoords);
	~ObjSurface();

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

private:
	mutable size_t m_faceSize;
	mutable size_t m_vertexSize;

	std::vector<float> m_Verts;
};

