#include "ObjSurface.h"
#include "../../Manager/MemoryMgr.h"


ObjSurface::ObjSurface(int sizeVert, double* vertices, double* normals): m_faceSize(0), m_vertexSize(0) {

	struct Vertex {
		vec3 Position;
		vec3 Normal;
	};

	float v0[] = {0, 0, 0};
	float n0[] = {0, 0, 0};
	float n1[] = {0, 0, 0};

	m_Verts.resize(sizeVert * 6);

	Vertex* vertex_tmp = reinterpret_cast<Vertex*>(&m_Verts[0]);

	for (int i = 0; i < sizeVert; ++i) {

		vertex_tmp->Position.x = static_cast<float>(*(vertices)++);
		vertex_tmp->Position.y = static_cast<float>(*(vertices)++);
		vertex_tmp->Position.z = static_cast<float>(*(vertices)++);

		vertex_tmp->Normal.x = static_cast<float>(*(normals)++);
		vertex_tmp->Normal.y = static_cast<float>(*(normals)++);
		vertex_tmp->Normal.z = static_cast<float>(*(normals)++);

		vertex_tmp++;
	}

	m_vertexSize = sizeVert;
}


ObjSurface::ObjSurface(int sizeVert, double* vertices, double* normals, double* texCoords): m_faceSize(0), m_vertexSize(0) {

	struct Vertex {
		vec3 Position;
		vec3 Normal;
		vec2 TexCoord;
	};

	m_Verts.resize(sizeVert * 8);

	Vertex* vertex_tmp = reinterpret_cast<Vertex*>(&m_Verts[0]);

	for (int i = 0; i < sizeVert; ++i) {
		vertex_tmp->Position.x = static_cast<float>(*(vertices)++);
		vertex_tmp->Position.y = static_cast<float>(*(vertices)++);
		vertex_tmp->Position.z = static_cast<float>(*(vertices)++);

		vertex_tmp->Normal.x = static_cast<float>(*(normals)++);
		vertex_tmp->Normal.y = static_cast<float>(*(normals)++);
		vertex_tmp->Normal.z = static_cast<float>(*(normals)++);

		if (texCoords == nullptr) {
			vertex_tmp->TexCoord.x = 0;
			vertex_tmp->TexCoord.y = 0;
		}
		else {
			vertex_tmp->TexCoord.x = static_cast<float>(*(texCoords)++);
			vertex_tmp->TexCoord.y = static_cast<float>(*(texCoords)++);
		}


		vertex_tmp++;
	}

	m_vertexSize = sizeVert;

}


ObjSurface::~ObjSurface() {

}


int ObjSurface::GetVertexCount() const {
	return m_vertexSize;
}


int ObjSurface::GetLineIndexCount() const {
	return 0;
}


int ObjSurface::GetTriangleIndexCount() const {
	return -1;
}


void ObjSurface::GenerateVertices(std::vector<float>& vertices, unsigned char flags) const {

	switch (flags) {
		case VertexFlagsNormals:
			vertices.resize(GetVertexCount() * 6); // xzy + xyz
			break;

		case VertexFlagsTexCoords:
			vertices.resize(GetVertexCount() * 8); // xzy + xyz + st
			break;

		default:
			return;
	}

	vertices = m_Verts;

}


void ObjSurface::GenerateLineIndices(std::vector<unsigned short>& indices) const {
}


void ObjSurface::GenerateTriangleIndices(std::vector<unsigned short>& indices) const {
}


vec3 ObjSurface::GenerateTopTriangle(vec3 v0, vec3 v1, vec3 v2) {

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


vec3 ObjSurface::GenerateBottomTriangle(vec3 v0, vec3 v1, vec3 v2) {

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


vec3 ObjSurface::LerpFilter(vec3 v0, vec3 v1, float kCoff) {

	vec3 v = v1 * kCoff + (v0 * (1.0f - kCoff));

	return v;
}


void ObjSurface::Exterminate() {
}


void ObjSurface::Destroy() {

	MemoryMgr::getInstance()->ReleaseObject(this);

}
