#include "MeshSurface.h"
#include "../../Manager/MemoryMgr.h"
// #include <iostream>

MeshSurface::MeshSurface() {}

MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals): m_faceSize(0), m_vertexSize(0), m_indexSize(-1) {

	MakeVertices(sizeVert, vertices, normals);
}


MeshSurface::MeshSurface(int sizeVert, float* vertices, float* normals, float* texCoords): m_faceSize(0), m_vertexSize(0), m_indexSize(-1) {

	MakeVertices(sizeVert, vertices, normals, texCoords);

}


MeshSurface::~MeshSurface() {

}

bool MeshSurface::MakeVertices(int sizeVert, float* vertices, float* normals, float* texCoords) {
	if(!m_Verts.empty()) return false;

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

		// std::cout << "{ " << vertex_tmp->Position.x << ", " << vertex_tmp->Position.y << ", " << vertex_tmp->Position.z << " }, ";
		

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

	// std::cout << "\n\n";
	m_vertexSize = sizeVert;
	return true;
}

bool MeshSurface::MakeIndices(int sizeIndic, int* indices) {
	if(!m_Indics.empty()) return false;

	struct Index {
		Vector3<unsigned short> index;
	};

	m_Indics.resize(sizeIndic * 3);

	Index* index_tmp = reinterpret_cast<Index*>(&m_Indics[0]);
	// std::cout << "\nsizeIndic : " << sizeIndic << "\n";

	for(int i = 0; i < sizeIndic; ++i) {
		index_tmp->index.x = static_cast<unsigned short>(*(indices)++);
		index_tmp->index.y = static_cast<unsigned short>(*(indices)++);
		index_tmp->index.z = static_cast<unsigned short>(*(indices)++);

		// std::cout << "{ " << index_tmp->index.x << ", " << index_tmp->index.y << ", " << index_tmp->index.z << " }, ";
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

	vertices.resize(GetVertexCount() * 8); // xzy + xyz + st

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
}


void MeshSurface::Destroy() {

	MemoryMgr::getInstance()->ReleaseObject(this);

}

const std::vector<int>& MeshSurface::getJointIDs() const {
	return m_jointIDs;
}

void MeshSurface::setJointIDs(const std::vector<int>& m_jointIDs) {
	MeshSurface::m_jointIDs = m_jointIDs;
}

const std::vector<float>& MeshSurface::getWeights() const {
	return m_weights;
}

void MeshSurface::setWeights(const std::vector<float>& m_weights) {
	MeshSurface::m_weights = m_weights;
}
