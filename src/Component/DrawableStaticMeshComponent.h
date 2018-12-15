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
	bool SetMesh(const SISurface& meshSurface);

private:
	void CreateMeshBuffers(const SISurface& surface);

private:
	GLStaticMeshID m_meshId;

public:
	friend class RenderComponent;
};