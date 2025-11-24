#include "SGameObject.h"

#include <utility>

#include "../Manager/MemoryMgr.h"
#include "../Manager/GameObjectMgr.h"
#include "../Component/TransformComponent.h"
#include "../Component/CustomComponent.h"
#include "../Manager/EngineCore.h"
#include "../Util/Loader/XML/XML.h"
#include "../Util/SafeLog.h"

using namespace CSE;

SGameObject::SGameObject() {
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();
}

SGameObject::SGameObject(const std::string& name) {
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_name = name;
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();
}

SGameObject::SGameObject(const std::string& name, const std::string& hash) {
    SObject::SetHash(hash);
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_name = name;
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();
}

SGameObject::~SGameObject() = default;

void SGameObject::Init() {
    //    for (auto component : m_components) {
    //        if (component == nullptr) continue;
    //
    //        component->Init();
    //
    //    }
}

void SGameObject::Tick(float elapsedTime) {
    if (!isEnable) return;
    if (isPrefab()) return;
    UpdateComponent(elapsedTime);
}

void SGameObject::Exterminate() {
    {
        auto iter = m_components.begin();
        while (iter != m_components.end()) {
            auto component = *iter;
            m_components.erase(iter++);

            if (component == nullptr) continue;
            CORE->GetCore(MemoryMgr)->ReleaseObject(component);
        }
    }
    CORE->GetCore(GameObjectMgr)->Remove(this);
}

void SGameObject::Destroy() {
    {
        auto iter = m_components.begin();
        while (iter != m_components.end()) {
            auto component = *iter;
            m_components.erase(iter++);

            if (component == nullptr) continue;
            CORE->GetCore(MemoryMgr)->ReleaseObject(component);
        }
    }

    m_components.clear();

    if (m_parent != nullptr) {
        RemoveParent();
    }

    {
        auto iter = m_children.begin();
        while (iter != m_children.end()) {
            auto object = *iter;
            m_children.erase(iter++);

            if (object == nullptr) continue;
            object->Destroy();
        }
    }
    m_status = DESTROY;
    CORE->GetCore(GameObjectMgr)->AddDestroyObject(this);
}

void SGameObject::AddChild(SGameObject* object) {
    if (object == nullptr) return;
    object->RemoveParent();
    m_children.push_back(object);
    object->m_parent = this;
}

void SGameObject::RemoveChildren(bool isAllLevel) {
    for (const auto& child: m_children) {
        child->RemoveChildren(isAllLevel);
    }
    m_children.clear();
}

void SGameObject::RemoveChild(SGameObject* object) {
    if (object == nullptr) return;
    m_children.remove(object);
    object->m_parent = nullptr;
}

SGameObject* SGameObject::GetParent() const {
    return m_parent;
}

void SGameObject::SetParent(SGameObject* object) {
    object->AddChild(this);
}

void SGameObject::RemoveParent() {
    if (m_parent == nullptr) return;
    m_parent->RemoveChild(this);
}

const std::list<SGameObject*>& SGameObject::GetChildren() const {
    return m_children;
}

void SGameObject::AddComponent(SComponent* component) {
    m_components.push_back(component);
}

const std::list<SComponent*>& SGameObject::GetComponents() const {
    return m_components;
}

#ifndef CSE_GLOBAL_SCRIPT_DISABLED
HSQOBJECT SGameObject::GetCustomComponent(const char* className) const {
    const auto& customComponentRef = ReflectionRef<CustomComponent>();
    for (const auto& component: m_components) {
        if (component == nullptr) continue;
        if (customComponentRef.IsSameClass(component)) {
            const auto& customComponent = static_cast<CustomComponent*>(component);
            if (customComponent->SGetClassName() != className) continue;
            return customComponent->GetClassInstance().GetObject();
        }
    }
    auto obj = HSQOBJECT();
    obj._type = OT_NULL;

    return obj;
}
#endif

void SGameObject::DeleteComponent(SComponent* component) {
    m_components.remove(component);
}

SComponent* SGameObject::CreateComponent(const char* type) {
    SComponent* component = static_cast<SComponent*>(ReflectionObject::NewObject(type));
    if (component == nullptr) {
        SafeLog::LogErrf("ERROR: \'%s\' is undefined.", type);
        return nullptr;
    }
    component->SetGameObject(this);
    AddComponent(component);
    if (m_status == IDLE)
        component->Init();
    return component;
}

std::string SGameObject::GetID() const {
    std::string id;
    for (const SGameObject* node = const_cast<SGameObject*>(this);; node = node->GetParent()) {
        if (node == nullptr) break;

        id = node->GetName() + (id.empty() ? "" : "/") + id;
    }

    return id;
}

std::string SGameObject::GetID(const SComponent* component) const {
    if (component == nullptr) return "";

    const auto& object = component->GetGameObject();
    return object->m_hash + "?" + component->GetClassType();
}

SGameObject* SGameObject::Find(const std::string& name) const {
    return CORE->GetCore(GameObjectMgr)->Find(ConvertSpaceStr(name, true));
}


SGameObject* SGameObject::FindLocalByID(const std::string& id) {
    const auto& currentId = GetID();
    if (currentId == id) return this;

    for (const auto& child: m_children) {
        const auto& result = child->FindLocalByID(id);
        if (result != nullptr) return result;
    }
    return nullptr;
}

SGameObject* SGameObject::FindByID(const std::string& id) {
    return CORE->GetCore(GameObjectMgr)->FindByID(ConvertSpaceStr(id, true));
}

SGameObject* SGameObject::FindByHash(const std::string& hash) {
    return CORE->GetCore(GameObjectMgr)->FindByHash(ConvertSpaceStr(hash, true));
}

bool SGameObject::GetIsEnable() const {
    if (m_parent != nullptr) return m_parent->GetIsEnable() && isEnable;
    return isEnable;
}

void SGameObject::SetIsEnable(bool is_enable) {
    isEnable = is_enable;
}

void SGameObject::UpdateComponent(float elapsedTime) {
    for (const auto& component: m_components) {
        if (m_status < IDLE) return;
        if (component == nullptr) continue;

        if (m_status == INIT) {
            component->Init();
            continue;
        }

        if (component->GetIsEnable())
            component->Tick(elapsedTime);
    }

    if (m_status == INIT) m_status = IDLE;
}

void SGameObject::SetUndestroyable(bool enable) {
    SObject::SetUndestroyable(enable);

    for (const auto& component: m_components) {
        component->SetUndestroyable(enable);
    }

    for (const auto& child: m_children) {
        child->SetUndestroyable(enable);
    }
}

bool SGameObject::isPrefab(bool OnlyThisObject) const {
    if (m_isPrefab || m_parent == nullptr || OnlyThisObject) return m_isPrefab;

    return m_parent->isPrefab();
}

void SGameObject::SetIsPrefab(bool m_isPrefab) {
    SGameObject::m_isPrefab = m_isPrefab;
}

const std::string& SGameObject::GetResourceID() const {
    return m_resourceID;
}

void SGameObject::SetResourceID(const std::string& resID, bool setChildren) {
    m_resourceID = resID;

    if (setChildren) {
        for (const auto& child: m_children) {
            child->SetResourceID(resID, setChildren);
        }
    }
}

SComponent* SGameObject::GetSComponentByHash(const std::string& hash) const {
    const auto& object = CORE->GetCore(GameObjectMgr)->FindByHash(ConvertSpaceStr(hash, true));
    if (object == nullptr) return nullptr;

    std::string componentName = split(hash, '?')[1];

    for (const auto& component: object->m_components) {
        if (componentName == component->GetClassType()) {
            return component;
        }
    }
    return nullptr;
}

void SGameObject::SetHash(const std::string& hash) {
    const std::string prevHash = std::string(m_hash);
    SObject::SetHash(hash);
    CORE->GetCore(GameObjectMgr)->ChangeHash(prevHash, hash);
}