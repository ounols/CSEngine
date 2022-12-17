#include <algorithm>
#include "GameObjectMgr.h"
#include "MemoryMgr.h"
#include "EngineCore.h"

using namespace CSE;


GameObjectMgr::GameObjectMgr() = default;


GameObjectMgr::~GameObjectMgr() = default;


void GameObjectMgr::Init() {

    for (const auto& pair : m_objects) {
        const auto& object = pair.second;
        if (object == nullptr) continue;
        object->Init();
    }

}


void GameObjectMgr::Update(float elapsedTime) {
    auto iterator = m_objects.begin();
    while (iterator != m_objects.end()) {
        const auto& object = iterator->second;
        if (object == nullptr) continue;
        object->Tick(elapsedTime);
        ++iterator;
    }
}


void GameObjectMgr::DestroyQueuedObject() {
    if (m_destroyObjectsQueue.empty()) return;

    for (; !m_destroyObjectsQueue.empty(); m_destroyObjectsQueue.pop()) {
        const auto& object = m_destroyObjectsQueue.front();

        SContainerHash<SGameObject*>::Remove(object);
        CORE->GetCore(MemoryMgr)->ReleaseObject(object);
    }
}

void GameObjectMgr::AddDestroyObject(SGameObject* object) {
    if(object->GetStatus() != SGameObject::DESTROY) return;
    m_destroyObjectsQueue.push(object);
}

SGameObject* GameObjectMgr::Find(const std::string& name) const {
    for (const auto& pair : m_objects) {
        const auto& object = pair.second;
        if (object->GetName() == name)
            return object;
    }

    return nullptr;

}

SGameObject* GameObjectMgr::FindByID(const std::string& id) const {
    std::string obj_id = split(id, '?')[0];

    for (const auto& pair : m_objects) {
        const auto& object = pair.second;
        if (object->isPrefab()) continue;
        auto id_ = object->GetID();
        if (id_ == obj_id)
            return object;
    }

    return nullptr;
}

SGameObject* GameObjectMgr::FindByHash(const std::string& hash) const {
    std::string obj_hash = split(hash, '?')[0];
    return SContainerHash<SGameObject*>::Get(obj_hash);
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

SComponent* GameObjectMgr::FindComponentByHash(const std::string& hash) const {
    auto object = FindByHash(hash);
    if (object == nullptr) return nullptr;

    auto components = object->GetComponents();
    auto split_str = split(hash, '?');

    for (auto component : components) {
        if (split_str[1] == component->GetClassType()) {
            return component;
        }
    }
    return nullptr;
}
