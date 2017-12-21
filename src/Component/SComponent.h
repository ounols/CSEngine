#pragma once
#include "../SObject.h"
#include <string>
#include "../Object/SGameObject.h"

class SGameObject;

class SComponent : public SObject {
public:

	SComponent() {

	}


	virtual ~SComponent() {
	}

	virtual void Init() = 0;
	virtual void Tick(float elapsedTime) = 0;


	void SetGameObject(SGameObject* object) {
		gameObject = object;
	}

protected:
	SGameObject* gameObject = nullptr;
};

#define Transform static_cast<TransformComponent*>(gameObject->GetTransform())
