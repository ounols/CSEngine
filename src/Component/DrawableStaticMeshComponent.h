#pragma once
#include "SComponent.h"
class DrawableStaticMeshComponent : public SComponent {
public:
	DrawableStaticMeshComponent();
	~DrawableStaticMeshComponent();

	void Tick(float elapsedTime) override;

private:
	int m_vertexBuffer = -1;
	int m_indicesBuffer = -1;
	int m_vertexSize = -1;
	int m_indicesSize = -1;
};

