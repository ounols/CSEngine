#pragma once
#include "../SObject.h"
#include <string>
#include <map>
#include "../Object/SGameObject.h"
#include "SISComponent.h"


class SGameObject;

class SComponent : public SObject , public virtual SISComponent {
public:

	explicit SComponent(std::string classType) : m_classType(classType) {

	}

	SComponent(const SComponent& src) : SISComponent(src) {
        gameObject = src.gameObject;
        isEnable = src.isEnable;
        m_classType = src.m_classType;
	}


	virtual ~SComponent() {

	}

	virtual SComponent* Clone() {
		return nullptr;
	}

	virtual void CopyReference(SComponent* src, std::map<const char*, SComponent*> lists) {
		return;
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

private:
	int m_refId;
};

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
#define COMPONENT_CONSTRUCTOR(CLASSNAME) CLASSNAME::CLASSNAME() : SComponent(#CLASSNAME)