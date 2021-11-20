#include <algorithm>
#include "GameObjectMgr.h"
#include "MemoryMgr.h"
#include "EngineCore.h"

using namespace CSE;


GameObjectMgr::GameObjectMgr() = default;


GameObjectMgr::~GameObjectMgr() = default;


void GameObjectMgr::Init() {

    for (const auto& object : m_objects) {
        if (object == nullptr) continue;
        object->Init();
    }

}


void GameObjectMgr::Update(float elapsedTime) {
    auto iterator = m_objects.begin();
    while (iterator != m_objects.end()) {
        const auto& object = *iterator;
        if (object == nullptr) continue;
        object->Tick(elapsedTime);
        ++iterator;

        if(object->GetStatus() == SGameObject::DESTROY) {
            DeleteGameObject(object);
        }
    }
}


void GameObjectMgr::DeleteGameObject(SGameObject* object) {
    if (object == nullptr) return;

    auto iGameObj = std::find(m_objects.begin(), m_objects.end(), object);

    if (iGameObj != m_objects.end()) {
        m_size--;
        m_objects.erase(iGameObj);
        CORE->GetCore(MemoryMgr)->ReleaseObject(object);
    }
}


SGameObject* GameObjectMgr::Find(const std::string& name) const {

    for (auto object : m_objects) {
        if (object->GetName() == name)
            return object;
    }

    return nullptr;

}

SGameObject* GameObjectMgr::FindByID(const std::string& id) const {

    std::string obj_id = split(id, '?')[0];

    for (auto object : m_objects) {
        if (object->isPrefab()) continue;
        auto id_ = object->GetID();
        if (id_ == obj_id)
            return object;
    }

    return nullptr;
}

SComponent* GameObjectMgr::FindComponentByID(const std::string& id) const {
    auto object = FindByID(id);
    if (object == nullptr) return nullptr;

    auto components = object->GetComponents();
    auto split_str = split(id, '?');

    for (auto component : components) {
        if (split_str[1] == component->GetClassType()) {
            return component;
        }
    }

    return nullptr;
}
