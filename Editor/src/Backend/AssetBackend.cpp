#include "AssetBackend.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Util/AssetsDef.h"
#include "../../src/Util/MoreString.h"
#include "../../src/Util/Render/STexture.h"

using namespace CSEditor;

AssetBackend& AssetBackend::GetInstance() {
    static AssetBackend instance;
    return instance;
}

AssetBackend::AssetMap AssetBackend::RefreshAssetsDirect() {
    AssetMap assets;
    const auto& assetsList = EEngineCore::getEditorInstance()->GetResMgrCore()->GetAllAssetReferences();
    for (const auto& asset : assetsList) {
        assets[asset->path].push_back(asset);
    }
    return assets;
}

const AssetBackend::AssetsVector* AssetBackend::GetFolderAssetsDirect(
    const std::string& folderPath,
    const AssetMap& assetMap) {

    const auto iter = assetMap.find(folderPath);
    if (iter == assetMap.end()) {
        return nullptr;
    }
    return &iter->second;
}

std::vector<std::string> AssetBackend::ParsePathComponentsDirect(
    const std::string& currentPath,
    const std::string& basePath) {

    if (currentPath.size() < basePath.size()) {
        return {};
    }

    std::string relativePath = currentPath.substr(basePath.size());
    return CSE::split(relativePath, '/');
}

void* AssetBackend::LoadAssetPreviewDirect(CSE::AssetMgr::AssetReference* asset) {
    if (!asset) return nullptr;

    // Check if already loaded
    if (dynamic_cast<CSE::STexture*>(asset->resource)) {
        return (void*)static_cast<CSE::STexture*>(asset->resource)->GetTextureID();
    }

    // Load texture preview if asset is a texture
    if (asset->class_type == "STexture") {
        asset->resource = CSE::SResource::Create<CSE::STexture>(asset);
        if (asset->resource) {
            return (void*)static_cast<CSE::STexture*>(asset->resource)->GetTextureID();
        }
    }

    return nullptr;
}

void AssetBackend::ReleaseAssetPreviewDirect(CSE::AssetMgr::AssetReference* asset) {
    if (!asset || !asset->resource) return;

    CORE->GetCore(ResMgr)->Remove(asset->resource);
    asset->resource = nullptr;
}

void* AssetBackend::GetLoadedPreviewDirect(CSE::AssetMgr::AssetReference* asset) {
    if (!asset) return nullptr;

    if (dynamic_cast<CSE::STexture*>(asset->resource)) {
        return (void*)static_cast<CSE::STexture*>(asset->resource)->GetTextureID();
    }

    return nullptr;
}
