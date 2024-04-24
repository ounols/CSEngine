#pragma once

#include <vector>
#include <unordered_map>
#include "Base/WindowBase.h"
#include "../../src/Manager/AssetMgr.h"

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

    private:
        void OnDragDrop(const CSE::AssetMgr::AssetReference& asset);

        bool OnAssetClickEvent(const CSE::AssetMgr::AssetReference& asset);

    private:
        std::string m_targetPath;

        std::unordered_map<std::string, AssetsVector> m_assets;
        AssetsVector* m_selectedFolder = nullptr;
        std::string m_currentPath;

        std::vector<std::string> m_pathSelector;
    };
}