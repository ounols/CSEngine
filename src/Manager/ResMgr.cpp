#include "ResMgr.h"
#include "CameraMgr.h"
#include "MemoryMgr.h"

using namespace CSE;

IMPLEMENT_SINGLETON(ResMgr);

ResMgr::ResMgr() {

    Init();

}


ResMgr::~ResMgr() {
    Exterminate();
}


void ResMgr::Init() {
    //Make Asset Manager
    m_assetManager = new AssetMgr();


    //Load Assets
    m_assetManager->LoadAssets(false);

}


void ResMgr::Exterminate() {
    SAFE_DELETE(m_assetManager);
}

void ResMgr::Register(SResource* m_object) {
    m_resources.push_back(m_object);
}

void ResMgr::Remove(SResource* m_object) {
    auto iObj = std::find(m_resources.begin(), m_resources.end(), m_object);

    if (iObj != m_resources.end()) {
        m_resources.erase(iObj);
    }

    MemoryMgr::getInstance()->ReleaseObject(m_object, true);
}

int ResMgr::GetSize() const {
    return m_resources.size();
}

bool ResMgr::IsEmpty() const {
    return m_resources.empty();
}

int ResMgr::GetID(SResource* object) const {
    auto it = std::find(m_resources.begin(), m_resources.end(), object);
    if (it == m_resources.end())
        return -1;
    else
        return std::distance(m_resources.begin(), it);
}

std::string ResMgr::RemoveDuplicatingName(std::string name) const {
    for (auto resource : m_resources) {
        if (name == resource->GetName()) return RemoveDuplicatingName(name + " of another");
    }

    return name;
}

AssetMgr::AssetReference* ResMgr::GetAssetReference(std::string name) const {
    return m_assetManager->GetAsset(name);
}

SResource* ResMgr::GetSResource(std::string name) const {
    for (auto res : m_resources) {
        if (res->GetName() == name) return res;
        if (res->GetID() == name) return res;
    }
    return nullptr;
}


#ifdef __ANDROID__

void ResMgr::SetAssetManager(AAssetManager* obj) {
    m_assetManager->SetAssetManager(obj);
}


AAssetManager* ResMgr::GetAssetManager() {
    return m_assetManager->GetAssetManager();
}

void ResMgr::SetEnv(JNIEnv *obj) {
    m_assetManager->SetEnv(obj);
}

JNIEnv *ResMgr::GetEnv() {
    return m_assetManager->GetEnv();
}

#endif
