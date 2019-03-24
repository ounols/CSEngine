#pragma once
#include "SComponent.h"
#include "Util/Render/MeshSurface.h"

class DrawableStaticMeshComponent : public SComponent {
public:
	DrawableStaticMeshComponent();
	virtual ~DrawableStaticMeshComponent();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Exterminate() override;

	SComponent* Clone(SGameObject* object) override;

	bool SetMesh(const SISurface& meshSurface);

protected:
	virtual void CreateMeshBuffers(const SISurface& surface);

protected:
	GLStaticMeshID m_meshId;

public:
	friend class RenderComponent;
};