#pragma once
#include "../SObject.h"
#include <string>
#include "../Object/SGameObject.h"
#include "SISComponent.h"

class SGameObject;

class SComponent : public SObject , public virtual SISComponent {
public:

	SComponent() {

	}


	virtual ~SComponent() {

	}


	void SetGameObject(SGameObject* object) {
		gameObject = object;
	}

	virtual SGameObject* GetGameObject() const {
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
