#pragma once
#include <vector>
#include "../../SObject.h"
#include "../../Manager/ResMgr.h"
#include "../../Manager/SurfaceMeshContainer.h"
#include "../Matrix.h"

struct GLMeshID {
	int m_vertexBuffer = -1;
	int m_indexBuffer = -1;
	int m_vertexSize = -1;
	int m_indexSize = -1;

	int m_jointId = -1;
	int m_weight = -1;
};


class SISurface : public SObject {
public:
	
	SISurface() {
		ResMgr::getInstance()->Register<SurfaceMeshContainer, SISurface>(this);
		SetUndestroyable(true);
	}
	virtual ~SISurface() {
	}

	virtual int GetVertexCount() const = 0;
	virtual int GetLineIndexCount() const = 0;
	virtual int GetTriangleIndexCount() const = 0;
	virtual void GenerateVertices(std::vector<float>& vertices, unsigned char flags = 0) const = 0;
	virtual void GenerateLineIndices(std::vector<unsigned short>& indices) const = 0;
	virtual void GenerateTriangleIndices(std::vector<unsigned short>& indices) const = 0;
	
public:
	mutable GLMeshID m_meshId;

};

class SIRender {
public:
	SIRender() {}
	virtual ~SIRender() {}

	virtual void SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection) = 0;
	virtual void Render(float elapsedTime) = 0;

protected:
	GLProgramHandle* handler = nullptr;
	bool isRenderActive = false;

public:
	friend class RenderContainer;
	friend class RenderMgr;
};