#include "SGameObject.h"

#include "../Manager/MemoryMgr.h"
#include "../Manager/GameObjectMgr.h"
#include "../Component/TransformComponent.h"
#include "../Component/CustomComponent.h"
#include "../Manager/EngineCore.h"

using namespace CSE;

SGameObject::SGameObject() {
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();
}

SGameObject::SGameObject(const SGameObject& src) {
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();

}


SGameObject::SGameObject(std::string name) {
    CORE->GetCore(GameObjectMgr)->Register(this);
    m_name = name;
    m_transform = CreateComponent<TransformComponent>();

    SGameObject::Init();
}


SGameObject::~SGameObject() {
}


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
    CORE->GetCore(GameObjectMgr)->Remove(this);
}


void SGameObject::Destroy() {

    for (auto component : m_components) {
        if (component == nullptr) continue;
        CORE->GetCore(MemoryMgr)->ReleaseObject(component);
    }

    m_components.clear();

    if (m_parent != nullptr) {
        RemoveParent();
    }

    CORE->GetCore(GameObjectMgr)->DeleteGameObject(this);

    for (auto object : m_children) {
        if (object == nullptr) continue;
        object->Destroy();
    }
}

void SGameObject::AddChild(SGameObject* object) {
    if (object == nullptr) return;
    m_children.push_back(object);
    object->m_parent = this;
}

void SGameObject::RemoveChild(bool isAllLevel) {
    for (auto child : m_children) {
        child->RemoveChild(isAllLevel);
    }
    m_children.clear();
}


void SGameObject::RemoveChild(SGameObject* object) {
    if (object == nullptr) return;
    auto ob = std::find(m_children.begin(), m_children.end(), object);
    m_children.erase(ob);
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


std::vector<SGameObject*> SGameObject::GetChildren() const {
    return m_children;
}


void SGameObject::AddComponent(SComponent* component) {

    auto str_class = component->GetClassType();
    int index = 0;
    for (const auto& comp : m_components) {
        if (comp->GetClassType() == str_class) {
            index++;
        }
    }

    if (index > 0) component->SetClassType(str_class + "?" + std::to_string(index));

    component->SetGameObject(this);
    m_components.push_back(component);

}


std::vector<SComponent*> SGameObject::GetComponents() const {
    return std::vector<SComponent*>(m_components);
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

    auto iCompObj = std::find(m_components.begin(), m_components.end(), component);

    if (iCompObj != m_components.end()) {
        m_components.erase(iCompObj);
        CORE->GetCore(MemoryMgr)->ReleaseObject(component);
        return true;
    }

    return false;

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

    int id = 0;

    for (const auto& comp : m_components) {
        if (component == comp) {
            return GetID() + '?' + component->GetClassType();
        }
        id++;
    }

    return "";
}


SGameObject* SGameObject::Find(std::string name) const {

    return CORE->GetCore(GameObjectMgr)->Find(ConvertSpaceStr(name, true));

}

SGameObject* SGameObject::FindByID(std::string id) {
    return CORE->GetCore(GameObjectMgr)->FindByID(ConvertSpaceStr(id, true));
}


bool SGameObject::GetIsEnable() const {
    return isEnable;
}


void SGameObject::SetIsEnable(bool is_enable) {
    isEnable = is_enable;
    for (const auto& component : m_components) {
        if (component == nullptr) continue;
        component->SetIsEnable(is_enable);
    }
}


void SGameObject::UpdateComponent(float elapsedTime) {
    for (const auto& component : m_components) {
        if (component == nullptr) continue;

        if (m_status == INIT) {
            component->Init();
            continue;
        }

        if (component->GetIsEnable())
            component->Tick(elapsedTime);
    }

    if (m_status == INIT)
        m_status = IDLE;

}

void SGameObject::SetUndestroyable(bool enable) {

    SObject::SetUndestroyable(enable);

    for (const auto& component : m_components) {
        component->SetUndestroyable(enable);
    }

    for (const auto& child : m_children) {
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

std::string SGameObject::GetResourceID() const {
    return m_resourceID;
}

void SGameObject::SetResourceID(std::string resID, bool setChildren) {
    m_resourceID = resID;

    if (setChildren) {
        for (const auto& child : m_children) {
            child->SetResourceID(resID, setChildren);
        }
    }
}

SComponent* SGameObject::GetSComponentByID(std::string id) const {
    return CORE->GetCore(GameObjectMgr)->FindComponentByID(id);
}



