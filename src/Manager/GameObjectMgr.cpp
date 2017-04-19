#include <algorithm>
#include "GameObjectMgr.h"
#include "MemoryMgr.h"

IMPLEMENT_SINGLETON(GameObjectMgr);

GameObjectMgr::GameObjectMgr() {}


GameObjectMgr::~GameObjectMgr() {}


void GameObjectMgr::Update(float elapsedTime) {
	for (const auto& object : m_gameObjects) {
		if (object == nullptr) continue;
		object->Tick(elapsedTime);
	}
}


void GameObjectMgr::DeleteGameObject(SGameObject* object) {
	if (object == nullptr) return;

	auto iGameObj = std::find(m_gameObjects.begin(), m_gameObjects.end(), object);

	if (iGameObj != m_gameObjects.end()) {
		m_size--;
		m_gameObjects.erase(iGameObj);
		MemoryMgr::getInstance()->ReleaseObject(object);
	}
}


SGameObject* GameObjectMgr::Find(std::string name) {

	for(auto object : m_gameObjects) {
		if (object->GetName() == name)
			return object;
	}

	return nullptr;

}
