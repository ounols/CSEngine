#include "AssetWindow.h"

#include <utility>
#include "../Manager/EEngineCore.h"
#include "../Objects/MainDocker.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Util/AssetsDef.h"
#include "../../src/Util/MoreString.h"
#include "../../src/Util/Loader/SCENE/SSceneLoader.h"

using namespace CSEditor;

AssetWindow::AssetWindow() {
    m_currentPath = CSE::AssetsPath();
}

AssetWindow::~AssetWindow() = default;

void AssetWindow::SetUI() {
    ImGui::Begin("Assets Explorer");

    if (m_assets.empty()) {
        RefreshAssets();
        RefreshExplorer();
        m_targetPath = CSE::AssetsPath();
    }

    {
        ImGui::BeginGroup();
        std::string targetPath = m_targetPath;
        if (ImGui::Button("Assets")) {
            ChangeCurrentPath(targetPath);
            RefreshExplorer();
            ImGui::EndGroup();
            ImGui::End();
            return;
        }
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        for (const auto& pathNode: m_pathSelector) {
            if (pathNode.empty()) continue;
            targetPath += pathNode + '/';
            if (ImGui::Button(pathNode.c_str())) {
                ChangeCurrentPath(targetPath);
                RefreshExplorer();
                ImGui::EndGroup();
                ImGui::End();
                return;
            }
            ImGui::SameLine();
            ImGui::Text(">");
            ImGui::SameLine();
        }
        ImGui::EndGroup();
    }

    ImGui::Separator();
    ImGui::BeginChild("aw_scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImVec2 button_sz(70, 70);
    ImGuiStyle& style = ImGui::GetStyle();
    int size = m_selectedFolder->size();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    for (int n = 0; n < size; n++) {
        const auto& asset = m_selectedFolder->at(n);
        ImGui::PushID(n);
        {
            ImGui::BeginGroup();
            ImGui::Button(asset->extension.c_str(), button_sz);
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + button_sz.x);
            ImGui::Text("%s", asset->name_full.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndGroup();
        }
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(0) &&
            OnAssetClickEvent(*asset)) {
            return;
        }
        // When Dragging
        OnDragDrop(*asset);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 =
                last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
        if (n + 1 < size && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::End();
}

void AssetWindow::RefreshAssets() {
    m_assets.clear();
    const auto& assetsList = EEngineCore::getEditorInstance()->GetResMgrCore()->GetAllAssetReferences();
    for (const auto& asset: assetsList) {
        m_assets[asset->path].push_back(asset);
    }
}

void AssetWindow::RefreshExplorer() {
    const auto& iter = m_assets.find(m_currentPath);
    if (iter == m_assets.end()) return;
    m_selectedFolder = &iter->second;
}

void AssetWindow::ChangeCurrentPath(std::string path) {
    m_currentPath = std::move(path);
    std::string path_str = m_currentPath.substr(m_targetPath.size());
    m_pathSelector = CSE::split(path_str, '/');
}

void AssetWindow::OnDragDrop(const CSE::AssetMgr::AssetReference& asset) {
//    if(asset.resource == nullptr) return;
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("AW_RES", &asset, sizeof(CSE::AssetMgr::AssetReference));
        ImGui::EndDragDropSource();
    }
}

bool AssetWindow::OnAssetClickEvent(const CSE::AssetMgr::AssetReference& asset) {
    if (asset.extension == "/\\?folder") {
        ChangeCurrentPath(asset.name_path + '/');
        RefreshExplorer();
    }
    else if (asset.extension == "scene" && !EEngineCore::getEditorInstance()->IsPreview()) {
        m_mainDocker->Reset();
        const auto& editorCore = EEngineCore::getEditorInstance();
        editorCore->SetCurrentScene(asset.name_path);
        editorCore->ResizePreviewCore();
        editorCore->InvokeEditorRender();

    }
    else {
        return false;
    }

    ImGui::PopID();
    ImGui::EndChild();
    ImGui::End();
    return true;
}
