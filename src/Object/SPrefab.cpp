//
// Created by ounols on 19. 2. 10.
//

#include "../Manager/SCloneFactory.h"
#include "../Manager/ResMgr.h"
#include "SPrefab.h"
#include "../Util/Loader/DAE/DAELoader.h"

using namespace CSE;

SPrefab::SPrefab() = default;

SPrefab::~SPrefab() = default;

void SPrefab::Exterminate() {

}

SGameObject* SPrefab::Clone(const vec3& position, SGameObject* parent) {

    GenerateResourceID();
    SGameObject* clone = SCloneFactory::Clone(m_root, parent);

    clone->GetTransform()->m_position = vec3(position);
    clone->SetIsPrefab(false);
    clone->SetUndestroyable(false);

    return clone;

}


SGameObject* SPrefab::Clone(const vec3& position, const vec3& scale, Quaternion rotation, SGameObject* parent) {

    GenerateResourceID();
    SGameObject* clone = SCloneFactory::Clone(m_root, parent);

    clone->GetTransform()->m_position = vec3(position);
    clone->GetTransform()->m_scale = vec3(scale);
    clone->GetTransform()->m_rotation = Quaternion(rotation);
    clone->SetIsPrefab(false);
    clone->SetUndestroyable(false);


    return clone;
}

bool SPrefab::SetGameObject(SGameObject* obj) {
    if (m_root != nullptr) return false;

    m_root = obj;
    m_root->SetUndestroyable(true);
    m_root->SetIsPrefab(true);
    m_root->SetResourceID(GetID(), true);


    return true;
}

void SPrefab::Init(const AssetMgr::AssetReference* asset) {

    AssetMgr::TYPE type = asset->type;


    switch (type) {
        case AssetMgr::DAE:
            DAELoader::GeneratePrefab(asset->path.c_str(), nullptr, nullptr, nullptr, this);
            break;
    }
}

void SPrefab::GenerateResourceID(SGameObject* obj) {

    if (obj == nullptr) {
        obj = m_root;
        obj->SetResourceID(GetID());
    } else {
        std::string resultID = obj->GetID();
        resultID = resultID.substr(resultID.find('/') + 1);
        obj->SetResourceID(std::string(GetID()) + "*" + resultID);
    }

    for (auto child : obj->GetChildren()) {
        GenerateResourceID(child);
    }

}

