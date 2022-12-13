//
// Created by ounols on 19. 6. 10.
//

#pragma once

#include <string>
#include <utility>
#include "../Manager/AssetMgr.h"
#include "../SObject.h"

namespace CSE {

    class SResource : public SObject {
    public:
        SResource();
        explicit SResource(bool isRegister);
        SResource(const SResource* resource, bool isRegister);

        ~SResource() override;

        void SetName(std::string name);

        void SetAbsoluteID(std::string id);

        std::string GetName() const {
            return m_name;
        }

        std::string GetAbsoluteID() const {
            return m_absoluteId;
        }

        void LinkResource(AssetMgr::AssetReference* asset) {
            SetResource(asset, false);
        }

        void LinkResource(std::string name) {
            SetResource(std::move(name), false);
        }

        template <class T>
        static T* Create(const std::string& name) {
            {
                SResource* res = GetResource(name);
                if (res != nullptr) return static_cast<T*>(res);
            }
            T* object = new T();
            SResource* res = object;

            res->SetResource(name);
            return object;
        }

        template <class T>
        static T* Create(const AssetMgr::AssetReference* asset) {
            if (asset == nullptr) return nullptr;
            {
                SResource* res = GetResource(asset->name);
                if (res != nullptr) return static_cast<T*>(res);
            }
            T* object = new T();
            SResource* res = object;

            res->SetResource(asset);
            return object;
        }

        template <class T>
        static T* Get(std::string name) {
            SResource* res = GetResource(std::move(name));
            if (res != nullptr) return static_cast<T*>(res);
            return nullptr;
        }

        void SetHash(std::string& hash) override;

    protected:
        virtual void Init(const AssetMgr::AssetReference* asset) = 0;

    private:
        void SetResource(std::string name, bool isInit = true);

        void SetResource(const AssetMgr::AssetReference* asset, bool isInit = true);

        static SResource* GetResource(std::string name);

    private:
        std::string m_name;
        std::string m_absoluteId;
        bool m_isInited = false;

    };
}