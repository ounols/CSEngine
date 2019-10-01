#include <algorithm>
#include "MemoryMgr.h"
#include "GameObjectMgr.h"
#include "../Util/MoreString.h"

IMPLEMENT_SINGLETON(GameObjectMgr);

GameObjectMgr::GameObjectMgr() {}


GameObjectMgr::~GameObjectMgr() {}


void GameObjectMgr::Init() {

	for (const auto& object : m_objects) {
		if (object == nullptr) continue;
		object->Init();
	}

}


void GameObjectMgr::Update(float elapsedTime) {
	for (const auto& object : m_objects) {
		if (object == nullptr) continue;
		object->Tick(elapsedTime);
	}
}


void GameObjectMgr::DeleteGameObject(SGameObject* object) {
	if (object == nullptr) return;

	auto iGameObj = std::find(m_objects.begin(), m_objects.end(), object);

	if (iGameObj != m_objects.end()) {
		m_size--;
		m_objects.erase(iGameObj);
		MemoryMgr::getInstance()->ReleaseObject(object);
	}
}


SGameObject* GameObjectMgr::Find(std::string name) const {

	for(auto object : m_objects) {
		if (object->GetName() == name)
			return object;
	}

	return nullptr;

}

SGameObject* GameObjectMgr::FindByID(std::string id) const {

    std::string obj_id = split(id, '?')[0];

    for(auto object : m_objects) {
        if(object->isPrefab()) continue;
        auto id_ = object->GetID();
        if (id_ == obj_id)
            return object;
    }

    return nullptr;
}
