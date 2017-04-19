#pragma once
#include "../SObject.h"
#include <vector>
#include "../Component/SComponent.h"
#include "../Util/Vector.h"

class SGameObject : public SObject {
public:
	SGameObject();
	explicit SGameObject(std::string name);
	~SGameObject();

	virtual void Tick(float elapsedTime);
	virtual void Exterminate() override;
	void Destroy();

	void AddComponent(SComponent* component);
	template <class T>
	T* GetComponent(const char* name);


	vec3 GetPosition() const {
		return m_position;
	}

	std::string GetName() const {
		return m_name;
	}


	void SetPosition(vec3 position) {
		m_position = position;
	}

	void SetName(std::string name) {
		m_name = name;
	}


private:
	void UpdateComponent(float elapsedTime);


private:
	std::vector<SComponent*> m_components;
	vec3 m_position;
	std::string m_name;

};


template <class T>
T* SGameObject::GetComponent(const char* name) {
	for (const auto& component : m_components) {
		if (component == nullptr) continue;

		if (!strcmp(component->getName(), name)) {
			return T(component);
		}
	}

	return nullptr;
}
