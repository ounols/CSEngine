#include "DrawableStaticMeshComponent.h"
#include "../OGLDef.h"
#include <vector>

#include <iostream>


COMPONENT_CONSTRUCTOR(DrawableStaticMeshComponent) {}


DrawableStaticMeshComponent::~DrawableStaticMeshComponent() {}


void DrawableStaticMeshComponent::Init() {
}


void DrawableStaticMeshComponent::Tick(float elapsedTime) {
}


void DrawableStaticMeshComponent::Exterminate() {

	// const GLuint vertexBuffer = m_meshId.m_vertexBuffer;
	// const GLuint indexBuffer = m_meshId.m_indexBuffer;

	// glDeleteBuffers(1, &vertexBuffer);
	// glDeleteBuffers(1, &indexBuffer);
}


bool DrawableStaticMeshComponent::SetMesh(const SISurface& meshSurface) {

	if (m_meshId.m_vertexSize != -1 || m_meshId.m_vertexBuffer != -1) return false;

	if (meshSurface.m_staticMeshId.m_vertexSize != -1
		|| meshSurface.m_staticMeshId.m_vertexBuffer != -1) {
		m_meshId = meshSurface.m_staticMeshId;
		return true;

	}

	CreateMeshBuffers(meshSurface);

	return true;
}


void DrawableStaticMeshComponent::CreateMeshBuffers(const SISurface& surface) {

	// Create the VBO for the vertices.
	std::vector<float> vertices;
	surface.GenerateVertices(vertices);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vertices[0]),
		&vertices[0],
		GL_STATIC_DRAW);


	// Create a new VBO for the indices if needed.
	int vertexCount = surface.GetVertexCount();
	int indexCount = surface.GetTriangleIndexCount();
	GLuint indexBuffer;


	// Set exception to not using indices when index count is 0 or lower.
	if (indexCount < 0) {
		indexBuffer = 0;
		indexCount = -1;

	} else {
		std::vector<GLushort> indices(indexCount);
		surface.GenerateTriangleIndices(indices);
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indexCount * 3 * sizeof(GLushort),
			&indices[0],
			GL_STATIC_DRAW);

	}


	//Pulling data
	surface.m_staticMeshId.m_vertexBuffer = vertexBuffer;
	surface.m_staticMeshId.m_vertexSize = vertexCount;
	surface.m_staticMeshId.m_indexBuffer = indexBuffer;
	surface.m_staticMeshId.m_indexSize = indexCount;

	m_meshId = surface.m_staticMeshId;

	//Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

SComponent* DrawableStaticMeshComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(DrawableStaticMeshComponent, comp);

    comp->m_meshId = m_meshId;
	return comp;
}