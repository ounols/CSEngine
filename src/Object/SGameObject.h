#pragma once
#include "../SObject.h"
#include <vector>
#include "../Component/SComponent.h"
#include "../Util/Vector.h"

class SGameObject :
	public SObject {
public:
	SGameObject();
	~SGameObject();

	virtual void Tick(float elapsedTime);
	virtual void Exterminate() override;
	void Destroy();

	void AddComponent(SComponent* component);
	template<class T>
	T* GetComponent(const char* name);

	
private:
	void UpdateComponent(float elapsedTime);


private:
	std::vector<SComponent*> m_components;
	vec3 m_position;

};


template <class T>
T* SGameObject::GetComponent(const char* name) {
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;

		if(!strcmp(component->getName(), name)) {
			return T(component);
		}
	}

	return nullptr;
}
