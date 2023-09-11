#include "AssetWindow.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Util/AssetsDef.h"

using namespace CSEditor;

AssetWindow::AssetWindow() {
    m_currentPath = CSE::AssetsPath();
}

AssetWindow::~AssetWindow() {

}

void AssetWindow::SetUI() {
    ImGui::Begin("Assets Explorer");

    if (m_assets.empty()) {
        RefreshAssets();
        RefreshExplorer();
    }

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
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && asset->extension == "/\\?folder") {
            m_currentPath = asset->name_path + '/';
            RefreshExplorer();
            ImGui::PopID();
            ImGui::End();
            return;
        }
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 =
                last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
        if (n + 1 < size && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }

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
