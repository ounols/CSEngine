#pragma once
#include "../SObject.h"
#include <string>
#include "../Object/SGameObject.h"
#include "SISComponent.h"

class SGameObject;

class SComponent : public SObject , public virtual SISComponent {
public:

	explicit SComponent(std::string classType) : m_classType(classType) {

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

	std::string GetClassType() const {
		return m_classType;
	}

protected:
	SGameObject* gameObject = nullptr;
	bool isEnable = true;

	std::string m_classType;
};

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
#define COMPONENT_CONSTRUCTOR(CLASSNAME) CLASSNAME::CLASSNAME() : SComponent(#CLASSNAME)