#include "SGameObject.h"
#include "../Manager/GameObjectMgr.h"
#include "../Component/TransformComponent.h"


SGameObject::SGameObject() {
	GameObjectMgr::getInstance()->RegisterGameObject(this);
	m_transform = CreateComponent<TransformComponent>();
}


SGameObject::SGameObject(std::string name) {
	GameObjectMgr::getInstance()->RegisterGameObject(this);
	m_name = name;
	m_transform = CreateComponent<TransformComponent>();
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


void SGameObject::UpdateComponent(float elapsedTime) {
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;

		component->Tick(elapsedTime);
	}
}


//bool SGameObject::isSameComponent(const SComponent* src, const SComponent* dst) {
//
//	
//
//}
