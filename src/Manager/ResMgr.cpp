#include "ResMgr.h"
#include "CameraMgr.h"
#include "MemoryMgr.h"
#include "EngineCore.h"
#include "../Util/AssetsDef.h"

#ifdef __ANDROID__
#include <Util/SafeLog.h>
#endif

using namespace CSE;

ResMgr::ResMgr() = default;

ResMgr::~ResMgr() {
    Exterminate();
}

void ResMgr::Init() {
    InitResource();
}

void ResMgr::InitResource() {
    //Make Asset Manager
    if(m_assetManager == nullptr)
        m_assetManager = new AssetMgr();

    //Load Assets
    m_assetManager->LoadAssets(Settings::IsAssetsPacked());
}

void ResMgr::Exterminate() {
    SAFE_DELETE(m_assetManager);
}

void ResMgr::Remove(SResource* m_object) {
    SContainerHash<SResource*>::Remove(m_object);
    CORE->GetCore(MemoryMgr)->ReleaseObject(m_object, true);
}

int ResMgr::GetStringHash(const std::string& str) {
	auto result = std::find(m_stringIds.begin(), m_stringIds.end(), str);
	if (result != m_stringIds.end()) 
		return result - m_stringIds.begin();

	m_stringIds.push_back(str);
	return m_stringIds.size() - 1;
}

AssetMgr::AssetReference* ResMgr::GetAssetReference(std::string name) const {
    return m_assetManager->GetAsset(name);
}

SResource* ResMgr::GetSResource(std::string name) const {
    if(m_objects.count(name) > 0) return Get(name);
    make_lower(name);
    for (const auto& pair : m_objects) {
        const auto& res = pair.second;
        if (make_lower_copy(res->GetName()) == name)
            return res;
        if (make_lower_copy(res->GetAbsoluteID()) == name)
            return res;
    }
    return nullptr;
}

std::list<AssetMgr::AssetReference*> ResMgr::GetAssetReferences(AssetMgr::TYPE type) const {
    return m_assetManager->GetAssets(type);
}

#ifdef __ANDROID__

void ResMgr::SetAssetManager(AAssetManager* obj) {
    SafeLog::Log("SetAssetManager");
    //Make Asset Manager
    if(m_assetManager == nullptr)
        m_assetManager = new AssetMgr();
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
