#include "SGameObject.h"
#include "../Manager/GameObjectMgr.h"


SGameObject::SGameObject() {
	GameObjectMgr::getInstance()->RegisterGameObject(this);
}


SGameObject::~SGameObject() {
	SGameObject::Exterminate();
}


void SGameObject::Tick(float elapsedTime) {
	UpdateComponent(elapsedTime);

}


void SGameObject::Exterminate() {
}


void SGameObject::Destroy() {
	GameObjectMgr::getInstance()->DeleteGameObject(this);
}


void SGameObject::AddComponent(SComponent* component) {
	m_components.push_back(component);
}


void SGameObject::UpdateComponent(float elapsedTime) {
	for (const auto& component : m_components) {
		if (component == nullptr)	continue;

		component->Tick(elapsedTime);
	}
}
