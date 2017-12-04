#pragma once
#include "SComponent.h"
#include "../Util/Render/ObjSurface.h"

class DrawableStaticMeshComponent : public SComponent {
public:
	DrawableStaticMeshComponent();
	~DrawableStaticMeshComponent();

	void Tick(float elapsedTime) override;
	void Exterminate() override;
	bool SetMesh(const SISurface& meshSurface, int flags);

private:
	void CreateMeshBuffers(const SISurface& surface, int flags);

private:
	int m_vertexBuffer = -1;
	int m_indexBuffer = -1;
	int m_vertexSize = -1;
	int m_indexSize = -1;
	int m_flags;
};

