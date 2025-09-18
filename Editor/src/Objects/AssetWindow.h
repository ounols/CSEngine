#pragma once

#include <vector>
#include <unordered_map>
#include <queue>
#include "Base/WindowBase.h"
#include "../../src/Manager/AssetMgr.h"

namespace CSE {
    class STexture;
}

namespace CSEditor {
    class AssetWindow : public WindowBase {
    private:
        typedef std::vector<CSE::AssetMgr::AssetReference*> AssetsVector;

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
        void OnDragDrop(const CSE::AssetMgr::AssetReference& asset);

        bool OnAssetClickEvent(const CSE::AssetMgr::AssetReference& asset);

        void ReleasePreviewQueue();

    private:
        std::string m_targetPath;

        std::unordered_map<std::string, AssetsVector> m_assets;
        AssetsVector* m_selectedFolder = nullptr;
        std::string m_currentPath;
        CSE::AssetMgr::AssetReference* m_currentSceneAsset = nullptr;
        std::queue<void*> m_previewAssetQueue;

        std::vector<std::string> m_pathSelector;
    };
}