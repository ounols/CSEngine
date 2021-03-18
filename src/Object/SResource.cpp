//
// Created by ounols on 19. 6. 10.
//

#include <iostream>
#include "SResource.h"
#include "../Manager/ResMgr.h"

using namespace CSE;

SResource::SResource() {
    ResMgr::getInstance()->Register(this);
    m_name = "Resource " + std::to_string(ResMgr::getInstance()->GetSize());
}

SResource::SResource(bool isRegister) {
    m_name = "Unregisted Resource";
}

SResource::SResource(const SResource* resource, bool isRegister) : SObject(isRegister) {
    if(isRegister) {
        ResMgr::getInstance()->Register(this);
    }
    m_name = resource->m_name + " (instance)";
}

SResource::~SResource() {

}

void SResource::SetName(std::string name) {
    m_name = ResMgr::getInstance()->RemoveDuplicatingName(name);
}

void SResource::SetResource(std::string name, bool isInit) {
    if (m_isInited) return;

    auto asset = ResMgr::getInstance()->GetAssetReference(name);

    SetResource(asset, isInit);
}

void SResource::SetResource(const AssetMgr::AssetReference* asset, bool isInit) {
    if (asset == nullptr) return;
    if (m_isInited) return;

    m_isInited = true;
    m_id = asset->id;

    SetName(asset->name);
    if (isInit)
        Init(asset);
}

SResource* SResource::GetResource(std::string name) {
    return ResMgr::getInstance()->GetSResource(name);
}
