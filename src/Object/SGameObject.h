#pragma once

#include <algorithm>
#include <list>
#include "../SObject.h"

#include "../Util/Interface/TransformInterface.h"
#include "../Component/SComponent.h"
#include "sqrat/sqratUtil.h"
#include "../Util/MoreString.h"

namespace CSE {

    class SComponent;

    class SGameObject : public SObject {
    private:
        enum STATUS {
            IDLE, INIT, DESTROY, UNKOWN
        };
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

        void RemoveChildren(bool isAllLevel = false);

        void RemoveChild(SGameObject* object);

        SGameObject* GetParent() const;

        void SetParent(SGameObject* object);

        void RemoveParent();

        const std::list<SGameObject*>& GetChildren() const;

        /**
         * \brief 컴포넌트를 이 오브젝트에 추가합니다.
         * \param component 추가할 오브젝트
         */
        void AddComponent(SComponent* component);

        template <class T>
        T* GetComponent();

        template <class T>
        T* GetComponentByID(std::string id) const;

        SComponent* GetSComponentByID(const std::string& id) const;

        const std::list<SComponent*>& GetComponents() const;

        HSQOBJECT GetCustomComponent(const char* className);

        void DeleteComponent(SComponent* component);

        template <class T>
        T* CreateComponent();

        SGameObject* Find(std::string name) const;

        static SGameObject* FindByID(std::string id);


        std::string GetName() const {
            return m_name;
        }

        std::string GetID() const;

        std::string GetID(const SComponent* component) const;

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
        std::list<SGameObject*> m_children;
        SGameObject* m_parent = nullptr;

        std::list<SComponent*> m_components;
        std::string m_name;
        TransformInterface* m_transform;
        bool isEnable = true;
        STATUS m_status = INIT;
        bool m_isPrefab = false;
        std::string m_resourceID;
    public:
        bool isPrefab(bool OnlyThisObject = false) const;

        void SetIsPrefab(bool m_isPrefab);

        std::string GetResourceID() const;

        void SetResourceID(const std::string& resID, bool setChildren = false);
    };


    template <class T>
    T* SGameObject::GetComponent() {
        for (const auto component : m_components) {
            if (component == nullptr) continue;
            if (dynamic_cast<T*>(component)) {
                return static_cast<T*>(component);
            }
        }

        return nullptr;
    }

    template <class T>
    T* SGameObject::GetComponentByID(std::string id) const {

        return static_cast<T*>(GetSComponentByID(id));
    }



    template <class T>
    T* SGameObject::CreateComponent() {

        T* component = new T();
        AddComponent(component);
        if (m_status == IDLE)
            component->Init();
        return component;

    }
}