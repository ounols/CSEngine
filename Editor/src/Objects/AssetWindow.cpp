#include "AssetWindow.h"

#include <utility>
#include "../Manager/EEngineCore.h"
#include "../Manager/EditorActionLogger.h"
#include "../Backend/AssetBackend.h"
#include "../Backend/SceneBackend.h"
#include "../Backend/EditorBackend.h"
#include "../Objects/MainDocker.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Util/AssetsDef.h"
#include "../../src/Util/MoreString.h"
#include "../../src/Util/Loader/SCENE/SSceneLoader.h"
#include "../../src/Util/Render/STexture.h"

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

    if (!RenderBreadcrumbNavigation()) return;

    ImGui::Separator();
    if (!RenderAssetGrid()) return;

    ImGui::End();
}

void AssetWindow::RefreshAssets() {
    m_assets = AssetBackend::GetInstance().RefreshAssetsDirect();
}

void AssetWindow::RefreshExplorer() {
    m_selectedFolder = AssetBackend::GetInstance().GetFolderAssetsDirect(m_currentPath, m_assets);
}

void AssetWindow::ChangeCurrentPath(std::string path) {
    m_currentPath = std::move(path);
    m_pathSelector = AssetBackend::GetInstance().ParsePathComponentsDirect(m_currentPath, m_targetPath);
}

void AssetWindow::OnDragDrop(CSE::AssetMgr::AssetReference* asset) {
    if (!asset) return;
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("AW_RES", &asset, sizeof(CSE::AssetMgr::AssetReference*));
        ImGui::EndDragDropSource();
    }
}

bool AssetWindow::OnAssetClickEvent(const CSE::AssetMgr::AssetReference& asset) {
    if (asset.extension == "/\\?folder") {
        ACTION_LOG_ASSET("Opened folder", asset.name_path);
        ChangeCurrentPath(asset.name_path + '/');
        RefreshExplorer();
    } else if (asset.extension == "scene" && !EditorBackend::GetInstance().IsPlaying()) {
        ACTION_LOG_ASSET("Opened scene", asset.name_path);
        m_mainDocker->Reset();
        m_currentSceneAsset = const_cast<CSE::AssetMgr::AssetReference*>(&asset);
        SceneBackend::GetInstance().LoadSceneDirect(asset.name_path);
    } else {
        return false;
    }

    ImGui::PopID();
    ImGui::EndChild();
    ImGui::End();
    return true;
}

void AssetWindow::ReleasePreviewQueue() {
    while (!m_previewAssetQueue.empty()) {
        auto* asset = m_previewAssetQueue.front();
        m_previewAssetQueue.pop();
        AssetBackend::GetInstance().ReleaseAssetPreviewDirect(asset);
    }
}

void AssetWindow::SaveCurrentScene() {
    if (m_currentSceneAsset == nullptr) {
        ACTION_LOG_SYSTEM(ActionSeverity::WARNING, "Save scene failed", "No scene is currently loaded");
        return;
    }
    SceneBackend::GetInstance().SaveSceneDirect(m_currentSceneAsset->name_path);
}

bool AssetWindow::RenderBreadcrumbNavigation() {
    ImGui::BeginGroup();
    std::string targetPath = m_targetPath;
    if (ImGui::Button("Assets")) {
        ChangeCurrentPath(targetPath);
        RefreshExplorer();
        ImGui::EndGroup();
        ImGui::End();
        return false;
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
            return false;
        }
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
    }
    ImGui::EndGroup();
    return true;
}

void* AssetWindow::GetAssetPreview(CSE::AssetMgr::AssetReference *asset, bool& isPreviewLoaded) {
    // Check if already loaded
    void* preview = AssetBackend::GetInstance().GetLoadedPreviewDirect(asset);
    if (preview) {
        return preview;
    }

    // Load if not yet loaded
    if (!isPreviewLoaded && asset->class_type == "STexture") {
        preview = AssetBackend::GetInstance().LoadAssetPreviewDirect(asset);
        if (preview) {
            m_previewAssetQueue.push(asset);
            isPreviewLoaded = true;
        }
    }
    return preview;
}

bool AssetWindow::RenderAssetGrid() {
    if (!m_selectedFolder) return true;

    ImGui::BeginChild("aw_scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImVec2 button_sz(70, 70);
    ImGuiStyle& style = ImGui::GetStyle();
    int size = m_selectedFolder->size();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    ImVec2 button_img_sz(65, 65);
    bool isPreviewLoaded = false;

    for (int n = 0; n < size; n++) {
        const auto& asset = m_selectedFolder->at(n);

        // Check if the prefab's inner resources.
        bool isDaeSubResource = asset->class_type == "Animation"
                                || asset->class_type == "Skeleton"
                                || asset->class_type == "MeshSurface";

        if (isDaeSubResource) {
            continue;
        }

        void* preview = GetAssetPreview(asset, isPreviewLoaded);

        ImGui::PushID(n);
        {
            ImGui::BeginGroup();
            if (preview) {
                ImGui::ImageButton(asset->extension.c_str(), preview, button_img_sz);
            } else {
                ImGui::Button(asset->extension.c_str(), button_sz);
            }
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + button_sz.x);
            ImGui::Text("%s", asset->name_full.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndGroup();
        }
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(0) &&
            OnAssetClickEvent(*asset)) {
            return false;
        }
        OnDragDrop(asset);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 =
                last_button_x2 + style.ItemSpacing.x + button_sz.x;
        if (n + 1 < size && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
    ImGui::EndChild();
    return true;
}
