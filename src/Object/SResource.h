//
// Created by ounols on 19. 6. 10.
//

#pragma once

#include <string>
#include <utility>
#include "../Manager/AssetMgr.h"
#include "../SObject.h"
#include "Base/ReflectionObject.h"
#include "../Util/VariableBinder.h"
#include "../Util/ResourceDef.h"

namespace CSE {

    class SResource : public SObject, public VariableBinder, public ReflectionObject {
    public:
        explicit SResource(const std::string& classType);

        SResource(const SResource* resource, bool isRegister);

        ~SResource() override;

        void SetName(const std::string& name);

        void SetAbsoluteID(const std::string& id);

        std::string GetName() const {
            return m_name;
        }

        std::string GetAbsoluteID() const {
            return m_absoluteId;
        }

        AssetMgr::AssetReference* GetAssetReference(const std::string& hash = "") const;

        void LinkResource(AssetMgr::AssetReference* asset) {
            SetResource(asset, false);
        }

        void LinkResource(const std::string& name) {
            SetResource(name, false);
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

        static SResource* Create(const std::string& name, const std::string& classType);

        template <class T>
        static T* Create(const AssetMgr::AssetReference* asset) {
            if (asset == nullptr) return nullptr;
            {
                SResource* res = GetResource(asset->hash);
                if (res != nullptr) return static_cast<T*>(res);
            }
            T* object = new T();
            SResource* res = object;

            res->SetResource(const_cast<AssetMgr::AssetReference*>(asset));
            return object;
        }

        static SResource* Create(const AssetMgr::AssetReference* asset, const std::string& classType);

        template <class T>
        static T* Get(const std::string& name) {
            SResource* res = GetResource(name);
            if (res != nullptr) return static_cast<T*>(res);
            return nullptr;
        }

        static SResource* Get(const std::string& name);

        void SetHash(const std::string& hash) override;

    protected:
        virtual void Init(const AssetMgr::AssetReference* asset) = 0;

    private:
        void SetResource(const std::string& name, bool isInit = true);

        void SetResource(AssetMgr::AssetReference* asset, bool isInit = true);

        static SResource* GetResource(const std::string& name);

    private:
        std::string m_name;
        std::string m_absoluteId;
        bool m_isInited = false;

    };
}