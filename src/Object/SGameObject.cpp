#include "SGameObject.h"

#include "../Manager/MemoryMgr.h"
#include "../Manager/GameObjectMgr.h"
#include "../Component/TransformComponent.h"
#include "../Component/CustomComponent.h"



SGameObject::SGameObject() {
	GameObjectMgr::getInstance()->Register(this);
	m_transform = CreateComponent<TransformComponent>();

	SGameObject::Init();
}


SGameObject::SGameObject(std::string name) {
	GameObjectMgr::getInstance()->Register(this);
	m_name = name;
	m_transform = CreateComponent<TransformComponent>();

	SGameObject::Init();
}


SGameObject::~SGameObject() {
}


void SGameObject::Init() {

	for(auto component : m_components) {
		if (component == nullptr) continue;

		component->Init();

	}

}


void SGameObject::Tick(float elapsedTime) {
	if (!isEnable) return;
	UpdateComponent(elapsedTime);

}


void SGameObject::Exterminate() {
	GameObjectMgr::getInstance()->Remove(this);
}


void SGameObject::Destroy() {

	for (auto component : m_components) {
		if (component == nullptr) continue;
		MemoryMgr::getInstance()->ReleaseObject(component);
	}

	m_components.clear();

	GameObjectMgr::getInstance()->DeleteGameObject(this);

	for(auto object : m_children) {
		if(object == nullptr) continue;
		object->Destroy();
	}
}

void SGameObject::AddChild(SGameObject* object) {
	if(object == nullptr) return;
	m_children.push_back(object);
}

std::vector<SGameObject*> SGameObject::GetChildren() const {
	return m_children;
}


void SGameObject::AddComponent(SComponent* component) {
	component->SetGameObject(this);
	m_components.push_back(component);

}


HSQOBJECT SGameObject::GetCustomComponent(const char* className) {

	for (auto component : m_components) {
		if (component == nullptr) continue;
		if (dynamic_cast<CustomComponent*>(component)) {
			auto customComponent = static_cast<CustomComponent*>(component);

			if (customComponent->SGetClassName() != className) continue;

			return customComponent->GetClassInstance().GetObject();


		}
	}

	HSQOBJECT obj = HSQOBJECT();
	obj._type = OT_NULL;

	return obj;
}


bool SGameObject::DeleteComponent(SComponent* component) {

	for (auto m_component : m_components) {
		if (m_component == nullptr) continue;

		if (std::addressof(component) == std::addressof(m_component)) {

			auto iCompObj = std::find(m_components.begin(), m_components.end(), m_component);

			if (iCompObj != m_components.end()) {
				m_components.erase(iCompObj);
				MemoryMgr::getInstance()->ReleaseObject(m_component);
			}


			return true;
		}
	}

	return false;

}


SGameObject* SGameObject::Find(std::string name) const {

	return GameObjectMgr::getInstance()->Find(name);

}


bool SGameObject::GetIsEnable() const {
	return isEnable;
}


void SGameObject::SetIsEnable(bool is_enable) {
	isEnable = is_enable;
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;
		component->SetIsEnable(is_enable);
	}
}


void SGameObject::UpdateComponent(float elapsedTime) {
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;

		if(component->GetIsEnable())
			component->Tick(elapsedTime);
	}
}
