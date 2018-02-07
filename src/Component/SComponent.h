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

	SGameObject* GetGameObject() const {
		return gameObject;
	}


	virtual bool GetIsEnable() const {
		return isEnable;
	}


	virtual void SetIsEnable(bool is_enable) {
		isEnable = is_enable;
	}


protected:
	SGameObject* gameObject = nullptr;
	bool isEnable = true;
};

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
