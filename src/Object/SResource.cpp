//
// Created by ounols on 19. 6. 10.
//

#include "SResource.h"
#include "../Manager/ResMgr.h"

SResource::SResource() {
    ResMgr::getInstance()->Register(this);
    m_name = "Resource " + std::to_string(ResMgr::getInstance()->GetSize());
}

SResource::~SResource() {

}

void SResource::SetName(std::string name) {
    m_name = ResMgr::getInstance()->RemoveDuplicatingName(name);
}

void SResource::SetResource(std::string name) {
    if(m_isInited) return;

    auto asset = ResMgr::getInstance()->GetAssetReference(name);

    SetResource(asset);
}

void SResource::SetResource(const AssetMgr::AssetReference* asset) {
    if(asset == nullptr) return;
    if(m_isInited) return;

    m_isInited = true;

    SetName(asset->name);
    Init(asset);
}

SResource* SResource::GetResource(std::string name) {
    return ResMgr::getInstance()->GetSResource(name);
}
