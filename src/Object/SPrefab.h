//
// Created by ounols on 19. 2. 10.
//

#pragma once

#include "../Object/SResource.h"
#include "../Util/Quaternion.h"
#include <string>

namespace CSE {

    class SGameObject;

    class SPrefab : public SResource {
    public:
        RESOURCE_DEFINE_CONSTRUCTOR(SPrefab);

        ~SPrefab() override;

        SGameObject* Clone(const vec3& position, SGameObject* parent = nullptr);

        SGameObject* Clone(const vec3& position, const vec3& scale, Quaternion rotation, SGameObject* parent = nullptr);

        bool SetGameObject(SGameObject* obj);

        void Exterminate() override;

        SGameObject* GetRoot() const {
            return m_root;
        }

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

        void GenerateResourceID(SGameObject* obj = nullptr);

    private:
        std::string GetMetaString(const SGameObject& object, unsigned int startIndex);
        std::string GenerateObjectMeta(const SGameObject& obj);

    private:
        SGameObject* m_root = nullptr;
    };

}