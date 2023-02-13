//
// Created by ounols on 19. 6. 10.
//

#include <iostream>
#include <utility>
#include "SResource.h"
#include "../Manager/ResMgr.h"
#include "../Manager/EngineCore.h"
#include "../Manager/MemoryMgr.h"
#include "../Util/AssetsDef.h"
#include "../Util/Loader/XML/XML.h"


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

SResource::~SResource() = default;

void SResource::SetName(std::string name) {
    m_name = std::move(name);
}

void SResource::SetAbsoluteID(std::string id) {
    m_absoluteId = std::move(id);
}

void SResource::SetResource(std::string name, bool isInit) {
    if (m_isInited) return;

    auto asset = CORE->GetCore(ResMgr)->GetAssetReference(std::move(name));

    SetResource(asset, isInit);
}

void SResource::SetResource(const AssetMgr::AssetReference* asset, bool isInit) {
    if (asset == nullptr) return;
    if (m_isInited) return;

    m_isInited = true;
    m_absoluteId = asset->id;
    if(!asset->hash.empty()) {
        std::string hash = asset->hash;
        SetHash(hash);
    }
    else {
        std::string meta = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<CSEMETA version=\"1.0.0\">\n"
                           "<hash-data hash=\"" + m_hash + "\">\n"
                           "\n</hash-data>\n</CSEMETA>";
        SaveTxtFile(asset->path + ".meta", meta);
    }

    SetName(asset->name);
    if (isInit)
        Init(asset);
}

SResource* SResource::GetResource(std::string name) {
    return CORE->GetCore(ResMgr)->GetSResource(std::move(name));
}

void SResource::SetHash(std::string& hash) {
    std::string srcHash = m_hash;
    SObject::SetHash(hash);
    CORE->GetCore(ResMgr)->ChangeHash(srcHash, hash);
}

AssetMgr::AssetReference* SResource::GetAssetReference(std::string hash) const {
    if(hash.empty()) hash = m_hash;
    return CORE->GetCore(ResMgr)->GetAssetReference(std::move(hash));
}
