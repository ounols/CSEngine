//
// Created by ounol on 2023-08-26.
//

#include "HierarchyWindow.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Object/SScene.h"

using namespace CSEditor;

HierarchyWindow::HierarchyWindow() {
    EEngineCore::getEditorInstance()->SetHierarchyData(this);
}

HierarchyWindow::~HierarchyWindow() {

}

void HierarchyWindow::SetUI() {
    ImGui::Begin("Hierarchy");

    RenderTrees();

    ImGui::End();
}

void HierarchyWindow::RenderTrees() {
    const auto& sceneMgr = CORE->GetSceneMgrCore();
    if (sceneMgr == nullptr) {
        m_selected = nullptr;
        return;
    }
    const auto& scene = dynamic_cast<CSE::SScene*>(sceneMgr->GetCurrentScene());
    if (scene == nullptr) {
        m_selected = nullptr;
        return;
    }
    const auto& root = scene->GetRoot();

    for (const auto& child: root->GetChildren()) {
        RenderGameObject(*child);
    }
}

void HierarchyWindow::RenderGameObject(CSE::SGameObject& parent) {
    const static ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_OpenOnDoubleClick
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_SpanFullWidth;

    const auto& name = parent.GetName();
    const auto& hash = parent.GetHash();
    const auto& children = parent.GetChildren();
    bool isOpen = ImGui::TreeNodeEx(hash.c_str(), flags | (children.empty() ? (ImGuiTreeNodeFlags_Leaf
                                                                               | ImGuiTreeNodeFlags_NoTreePushOnOpen)
                                                                            : NULL) |
                                                  (m_selected != nullptr && m_selected == &parent
                                                   ? ImGuiTreeNodeFlags_Selected : NULL), name.c_str());
    if (ImGui::IsItemClicked()) {
        m_selected = &parent;
    }
    if (ImGui::BeginDragDropSource()) {

        ImGui::EndDragDropSource();
    }

    if (isOpen) {
        for (const auto& child: children) {
            RenderGameObject(*child);
        }
        if (!children.empty()) {
            ImGui::TreePop();
        }
    }
}