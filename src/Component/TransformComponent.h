#pragma once
#include "SComponent.h"
#include "../Util/Interface/TransformInterface.h"
#include "../Util/Matrix.h"


class TransformComponent : public SComponent, public TransformInterface {
public:
	TransformComponent();
	~TransformComponent();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Exterminate() override;

	mat4 GetMatrix() const;
};

