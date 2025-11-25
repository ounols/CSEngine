#pragma once

#include <vector>
#include <unordered_map>
#include <queue>
#include "Base/WindowBase.h"
#include "../../src/Manager/AssetMgr.h"
#include "../Backend/AssetBackend.h"

namespace CSE {
    class STexture;
}

namespace CSEditor {
    class AssetWindow : public WindowBase {
    private:
        using AssetsVector = AssetBackend::AssetsVector;
        using AssetMap = AssetBackend::AssetMap;

    public:
        AssetWindow();
        ~AssetWindow() override;

        void SetUI() override;

        void RefreshAssets();

        void RefreshExplorer();

        void ChangeCurrentPath(std::string path);

        const std::string& GetCurrentPath() const {
            return m_currentPath;
        }

        void SaveCurrentScene();

    private:
        void OnDragDrop(CSE::AssetMgr::AssetReference* asset);

        bool OnAssetClickEvent(const CSE::AssetMgr::AssetReference& asset);

        void ReleasePreviewQueue();

        bool RenderBreadcrumbNavigation();

        void* GetAssetPreview(CSE::AssetMgr::AssetReference *asset, bool& isPreviewLoaded);

        bool RenderAssetGrid();

    private:
        std::string m_targetPath;

        AssetMap m_assets;
        const AssetsVector* m_selectedFolder = nullptr;
        std::string m_currentPath;
        CSE::AssetMgr::AssetReference* m_currentSceneAsset = nullptr;
        std::queue<CSE::AssetMgr::AssetReference*> m_previewAssetQueue;

        std::vector<std::string> m_pathSelector;
    };
}