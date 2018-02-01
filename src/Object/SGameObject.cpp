#include "SGameObject.h"

#include <memory>

#include "../Manager/GameObjectMgr.h"
#include "../Component/TransformComponent.h"


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
	SGameObject::Exterminate();
}


void SGameObject::Init() {

	for(auto component : m_components) {
		if (component == nullptr) continue;

		component->Init();

	}

}


void SGameObject::Tick(float elapsedTime) {
	UpdateComponent(elapsedTime);

}


void SGameObject::Exterminate() {
}


void SGameObject::Destroy() {

	for (auto component : m_components) {
		if (component == nullptr) continue;
		MemoryMgr::getInstance()->ReleaseObject(component);
	}

	m_components.clear();

	GameObjectMgr::getInstance()->DeleteGameObject(this);
}


void SGameObject::AddComponent(SComponent* component) {
	component->SetGameObject(this);
	m_components.push_back(component);

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


void SGameObject::UpdateComponent(float elapsedTime) {
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;

		if(component->getIsEnable())
			component->Tick(elapsedTime);
	}
}


//bool SGameObject::isSameComponent(const SComponent* src, const SComponent* dst) {
//
//	
//
//}
