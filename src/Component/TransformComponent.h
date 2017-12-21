#pragma once
#include "../Util/Vector.h"
#include "SComponent.h"

struct TransformInterface {
	vec3 m_position;
	vec3 m_rotation;
	vec3 m_scale;
};

class TransformComponent : public SComponent, public TransformInterface {
public:
	TransformComponent();
	~TransformComponent();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Exterminate() override;

	
};

