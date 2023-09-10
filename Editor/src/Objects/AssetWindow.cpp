#include "AssetWindow.h"
#include "../Manager/EEngineCore.h"

using namespace CSEditor;

AssetWindow::AssetWindow() {

}

AssetWindow::~AssetWindow() {

}

void AssetWindow::SetUI() {
    ImGui::Begin("Assets Explorer");

    ImVec2 button_sz(40, 40);
    ImGuiStyle& style = ImGui::GetStyle();
    int buttons_count = 20;
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    for (int n = 0; n < buttons_count; n++)
    {
        ImGui::PushID(n);
        ImGui::Button("Box", button_sz);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
        if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }

    ImGui::End();
}