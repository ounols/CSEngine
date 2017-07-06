#pragma once
#include "../SObject.h"
#include <vector>
#include "../Component/SComponent.h"
#include "../Component/TransformComponent.h"

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
	T* GetComponent();
	template <class T>
	bool DeleteComponent();

	std::string GetName() const {
		return m_name;
	}

	void SetName(std::string name) {
		m_name = name;
	}

	TransformComponent* GetTransform() const {
		return m_transform;
	}


private:
	void UpdateComponent(float elapsedTime);

private:
	std::vector<SComponent*> m_components;
	std::string m_name;
	TransformComponent* m_transform;
};


template <class T>
T* SGameObject::GetComponent() {
	for (const auto& component : m_components) {
		if (component == nullptr) continue;

		if (typeid(component) == typeid(T)) {
			return T(component);
		}
	}

	return nullptr;
}

template <class T>
bool SGameObject::DeleteComponent() {
	for (auto component : m_components) {
		if (component == nullptr) continue;

		if (typeid(component) == typeid(T)) {
			
			auto iCompObj = std::find(m_components.begin(), m_components.end(), component);

			if(iCompObj != m_components.end()) {
				m_components.erase(iCompObj);
				MemoryMgr::getInstance()->ReleaseObject(component);
			}
			

			return true;
		}
	}

	return false;
}