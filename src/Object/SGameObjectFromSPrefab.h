#pragma once

#include <utility>

#include "SGameObject.h"

namespace CSE {

    class SGameObjectFromSPrefab : public SGameObject {
    public:
        SGameObjectFromSPrefab() : SGameObject() {}
        explicit SGameObjectFromSPrefab(std::string name) : SGameObject(std::move(name)) {}
        explicit SGameObjectFromSPrefab(std::string name, std::string hash)
            : SGameObject(std::move(name), std::move(hash)) {}
        ~SGameObjectFromSPrefab() override = default;

        void SetRefHash(std::string hash) {
            m_refHash = std::move(hash);
        }

        std::string GetRefHash() const {
            return m_refHash;
        }

        SComponent* GetSComponentByRefHash(const std::string& hash) const {
            const auto& rawData = split(hash, '?');
            const auto& hashData = rawData[0];
            const auto& componentName = rawData[1];
            if(m_refHash == hashData) {
                const auto& components = GetComponents();
                for (const auto& component : components) {
                    if(componentName == component->GetClassType()) {
                        return component;
                    }
                }
                return nullptr;
            }

            const auto& children = GetChildren();
            for(const auto& child : children) {
                const auto& component = static_cast<SGameObjectFromSPrefab*>(child)->GetSComponentByRefHash(hash);
                if(component != nullptr)
                    return component;
            }
            return nullptr;
        }

        template <class T>
        T* GetComponentByRefHash(const std::string& hash) const;

        std::string GetRefID(const SComponent* component) const {
            if (component == nullptr) return "";

            const auto& object = static_cast<SGameObjectFromSPrefab*>(component->GetGameObject());
            return object->m_refHash + "?" + component->GetClassType();
        }

    private:
        std::string m_refHash;
    };

    template <class T>
    T* SGameObjectFromSPrefab::GetComponentByRefHash(const std::string& hash) const {
        return static_cast<T*>(GetSComponentByRefHash(hash));
    }
}
