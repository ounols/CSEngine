#pragma once
#include "SComponent.h"
#include "../Util/Vector.h"

class TransformComponent : public SComponent {
public:
	TransformComponent();
	~TransformComponent();

	void Tick(float elapsedTime) override;
	void Exterminate() override;

public:
	vec3 m_position;
	vec3 m_rotation;
	vec3 m_scale;
};

