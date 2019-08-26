#pragma once

#include <vector>
#include <algorithm>
#include "../SObject.h"

#include "../Util/Interface/TransformInterface.h"
#include "../Component/SComponent.h"
#include "sqrat/sqratUtil.h"
#include "../Util/MoreString.h"

class SComponent;

class SGameObject : public SObject {
private:
    enum STATUS { IDLE, INIT, DESTROY, UNKOWN };
public:
    SGameObject();
    SGameObject(const SGameObject& src);
    explicit SGameObject(std::string name);

    ~SGameObject();

    virtual void Init();

    virtual void Tick(float elapsedTime);

    virtual void Exterminate() override;

    /**
     * \brief 자동 삭제가 아닌 특정한 상황에서 삭제될 때 호출되는 함수
     */
    void Destroy();

    void SetUndestroyable(bool enable) override;

    void AddChild(SGameObject* object);
    void RemoveChild(bool isAllLevel = false);
    void RemoveChild(SGameObject* object);

    SGameObject* GetParent() const;
    void SetParent(SGameObject* object);
    void RemoveParent();

    std::vector<SGameObject*> GetChildren() const;

    /**
     * \brief 컴포넌트를 이 오브젝트에 추가합니다.
     * \param component 추가할 오브젝트
     */
    void AddComponent(SComponent* component);

    template<class T>
    T* GetComponent();
    template<class T>
    T* GetComponentByID(std::string id) const;
    std::vector<SComponent*> GetComponents() const;
    HSQOBJECT GetCustomComponent(const char* className);

    //template <class T>
    //bool DeleteComponent();
    bool DeleteComponent(SComponent* component);

    template<class T>
    T* CreateComponent();

    SGameObject* Find(std::string name) const;
    static SGameObject* FindByID(std::string id);



    std::string GetName() const {
        return m_name;
    }

    std::string GetID() const;
    std::string GetID(SComponent* component) const;

    void SetName(std::string name) {
        m_name = name;
    }

    TransformInterface* GetTransform() const {
        return m_transform;
    }


    bool GetIsEnable() const;

    void SetIsEnable(bool is_enable);

private:
    void UpdateComponent(float elapsedTime);


private:
    std::vector<SGameObject*> m_children;
    SGameObject* m_parent = nullptr;

    std::vector<SComponent*> m_components;
    std::string m_name;
    TransformInterface* m_transform;
    bool isEnable = true;
    STATUS m_status = INIT;
    bool m_isPrefab = false;
public:
    bool isPrefab() const;

    void SetIsPrefab(bool m_isPrefab);
};


template<class T>
T* SGameObject::GetComponent() {
    for (auto component : m_components) {
        if (component == nullptr) continue;
        if (dynamic_cast<T*>(component)) {
            return static_cast<T*>(component);
        }
    }

    return nullptr;
}

template <class T>
T* SGameObject::GetComponentByID(std::string id) const {

    auto object = FindByID(id);
    if(object == nullptr) return nullptr;

    auto components = object->GetComponents();
    auto split_str = split(id, '&');

    for (auto component : components) {
        std:: string comp_id = GetID(component);
        if(id == comp_id) {
            return static_cast<T*>(component);
        }
    }

    return nullptr;
}

//로직 및 문법 상 문제가 제기됨
//template <class T>
//bool SGameObject::DeleteComponent() {
//	for (auto component : m_components) {
//		if (component == nullptr) continue;
//		
//		if (dynamic_cast<T*>(component)) {
//			
//			auto iCompObj = std::find(m_components.begin(), m_components.end(), component);
//
//			if(iCompObj != m_components.end()) {
//				m_components.erase(iCompObj);
//				//MemoryMgr::getInstance()->ReleaseObject((SObject*)component);
//			}
//			
//
//			return true;
//		}
//	}
//
//	return false;
//}


template<class T>
T* SGameObject::CreateComponent() {

    T* component = new T();
    AddComponent(component);
    if(m_status == IDLE)
        component->Init();
    return component;

}
