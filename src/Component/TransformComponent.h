#pragma once
#include "SComponent.h"
#include "../Util/Interface/TransformInterface.h"


class TransformComponent : public SComponent, public TransformInterface {
public:
	TransformComponent();
	~TransformComponent();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Exterminate() override;

	
};

