//
// Created by ounols on 19. 6. 10.
//

#pragma once

#include <string>
#include "../Manager/AssetMgr.h"
#include "../SObject.h"
#include "../Manager/MemoryMgr.h"

namespace CSE {

    class SResource : public SObject {
    public:
        SResource();
        SResource(bool isRegister);
        SResource(const SResource* resource, bool isRegister);

        virtual ~SResource();

        void SetName(std::string name);

        void SetID(std::string id);

        const char* GetName() const {
            return m_name.c_str();
        }

        const char* GetID() const {
            return m_id.c_str();
        }

        void LinkResource(AssetMgr::AssetReference* asset) {
            SetResource(asset, false);
        }

        void LinkResource(std::string name) {
            SetResource(name, false);
        }

        template <class T>
        static T* Create(std::string name, bool isForceCreate = false) {
            if (!isForceCreate) {
                SResource* res = GetResource(name);
                if (res != nullptr) return static_cast<T*>(res);
            }
            T* object = new T();
            SResource* res = object;

            res->SetResource(name);
            return object;
        }

        template <class T>
        static T* Create(const AssetMgr::AssetReference* asset, bool isForceCreate = false) {
            if (asset == nullptr) return nullptr;
            if (!isForceCreate) {
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
            SResource* res = GetResource(name);
            if (res != nullptr) return static_cast<T*>(res);
            return nullptr;
        }

    protected:
        virtual void Init(const AssetMgr::AssetReference* asset) = 0;

    private:
        void SetResource(std::string name, bool isInit = true);

        void SetResource(const AssetMgr::AssetReference* asset, bool isInit = true);

        static SResource* GetResource(std::string name);

    private:
        std::string m_name;
        std::string m_id;
        bool m_isInited = false;

    };
}