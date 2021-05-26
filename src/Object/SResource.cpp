//
// Created by ounols on 19. 6. 10.
//

#include <iostream>
#include "SResource.h"
#include "../Manager/ResMgr.h"
#include "../Manager/EngineCore.h"
#include "../Manager/MemoryMgr.h"


using namespace CSE;

SResource::SResource() {
    auto resMgr = CORE->GetCore(ResMgr);
    resMgr->Register(this);
    m_name = "Resource " + std::to_string(resMgr->GetSize());
}

SResource::SResource(bool isRegister) {
    m_name = "Unregister Resource";
}

SResource::SResource(const SResource* resource, bool isRegister) : SObject(isRegister) {
    if(isRegister) {
        CORE->GetCore(ResMgr)->Register(this);
    }
    m_name = resource->m_name + " (instance)";
}

SResource::~SResource() {

}

void SResource::SetName(std::string name) {
    m_name = CORE->GetCore(ResMgr)->RemoveDuplicatingName(name);
}

void SResource::SetID(std::string id) {
    m_id = id;
}

void SResource::SetResource(std::string name, bool isInit) {
    if (m_isInited) return;

    auto asset = CORE->GetCore(ResMgr)->GetAssetReference(name);

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
    return CORE->GetCore(ResMgr)->GetSResource(name);
}
