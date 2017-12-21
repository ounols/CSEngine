#pragma once
#include "SComponent.h"
#include "../Util/Render/ObjSurface.h"

class DrawableStaticMeshComponent : public SComponent {
public:
	DrawableStaticMeshComponent();
	~DrawableStaticMeshComponent();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Exterminate() override;
	bool SetMesh(const SISurface& meshSurface, int flags);

private:
	void CreateMeshBuffers(const SISurface& surface, int flags);

private:
	GLStaticMeshID m_meshId;

public:
	friend class RenderComponent;
};