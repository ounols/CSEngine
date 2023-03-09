//
// Created by ounols on 19. 2. 10.
//

#include "../Manager/SCloneFactory.h"
#include "../Manager/ResMgr.h"
#include "SPrefab.h"
#include "../Util/Loader/DAE/DAELoader.h"
#include "../Util/AssetsDef.h"
#include "SGameObjectFromSPrefab.h"

using namespace CSE;

SPrefab::SPrefab() = default;

SPrefab::~SPrefab() = default;

void SPrefab::Exterminate() {
}

SGameObject* SPrefab::Clone(const vec3& position, SGameObject* parent) {
    SGameObject* clone = SCloneFactory::Clone(m_root, parent);

    clone->GetTransform()->m_position = vec3(position);
    clone->SetIsPrefab(false);
    clone->SetUndestroyable(false);

    return clone;
}


SGameObject* SPrefab::Clone(const vec3& position, const vec3& scale, Quaternion rotation, SGameObject* parent) {
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
    m_root->SetResourceID(GetHash(), true);

    return true;
}

void SPrefab::Init(const AssetMgr::AssetReference* asset) {
    AssetMgr::TYPE type = asset->type;
    std::string path = asset->name_path;

    switch (type) {
        case AssetMgr::DAE:
            DAELoader::GeneratePrefab(path.c_str(), nullptr, nullptr, nullptr, this);
            break;
    }

    std::string hashRaw = AssetMgr::LoadAssetFile(path + ".meta");
    if(!hashRaw.empty()) {
        const XNode* root = XFILE().loadBuffer(hashRaw);
        const auto& hashData = root->getNode("hash-data");
        std::string hash = hashData.getAttribute("hash").value;
        SetHash(hash);
        const auto& hashChildren = hashData.children;

        if(hashData.children.size() <= 0) {
            hashRaw = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                      "<CSEMETA version=\"1.0.0\">\n"
                      "<hash-data hash=\"" + m_hash + "\">\n"
                      + m_root->GenerateMeta() +
                      "\n</hash-data>\n</CSEMETA>";
            SaveTxtFile(path + ".meta", hashRaw);
        }
        else {
            for(const auto& child : hashChildren) {
                const auto& id = child.getAttribute("id").value;
                std::string hash = child.value;
                const auto& obj = static_cast<SGameObjectFromSPrefab*>(m_root->FindLocalByID(id));
                if(obj == nullptr) continue;
                obj->SetHash(hash);
                obj->SetRefHash(hash);
            }
        }
        SAFE_DELETE(root);
    }
    GenerateResourceID();
}

void SPrefab::GenerateResourceID(SGameObject* obj) {
    if (obj == nullptr) {
        obj = m_root;
        obj->SetResourceID(GetHash());
    } else {
        std::string resultID = obj->GetID();
        resultID = resultID.substr(resultID.find('/') + 1);
        obj->SetResourceID(std::string(GetHash()) + "*" + resultID);
    }

    for (auto child : obj->GetChildren()) {
        GenerateResourceID(child);
    }
}