//
// Created by ounol on 2023-08-26.
//

#include "HierarchyWindow.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Object/SScene.h"
#include "../../src/Object/SPrefab.h"
#include "../../../src/Manager/ResMgr.h"

using namespace CSEditor;

HierarchyWindow::HierarchyWindow() {
    m_core = EEngineCore::getEditorInstance();
    m_core->SetHierarchyData(this);
}

HierarchyWindow::~HierarchyWindow() = default;

void HierarchyWindow::SetUI() {
    ImGui::Begin("Hierarchy");

    RenderTrees();

    if (!m_core->IsPreview() && (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())) {
        for (ImGuiKey key = static_cast<ImGuiKey>(0); key < ImGuiKey_COUNT; key = (ImGuiKey) (key + 1)) {
            if (ImGui::IsKeyDown(key)) {
                m_core->InvokeEditorRender();
                break;
            }
        }
    }

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
    if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) {
        m_selected = &parent;
    }
    // When Dragging
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("INSP_GOBJ", &parent, sizeof(CSE::SGameObject));
        ImGui::EndDragDropSource();
    }
    // When Dropped
    if (ImGui::BeginDragDropTarget()) {
        if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_RES")) {
            const auto& dropObject = static_cast<CSE::SResource*>(payload->Data);
            if (dropObject->IsSameClass("SPrefab")) {
                const auto& obj = static_cast<CSE::SPrefab*>(dropObject)->Clone(CSE::vec3::Zero, &parent);
                UpdateGameObject(*obj);
            }
        }
        if (const auto& payload = ImGui::AcceptDragDropPayload("AW_RES")) {
            const auto& dropObject = static_cast<CSE::AssetMgr::AssetReference*>(payload->Data);
            if ("SPrefab" == dropObject->class_type) {
                const auto& res = CSE::SResource::Create<CSE::SPrefab>(dropObject);
                const auto& obj = res->Clone(CSE::vec3::Zero, &parent);
                UpdateGameObject(*obj);
            }
        }
        ImGui::EndDragDropTarget();
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

void HierarchyWindow::UpdateGameObject(CSE::SGameObject& parent) {
    parent.Tick(0);
    for (const auto& child : parent.GetChildren()) {
        UpdateGameObject(*child);
    }
}
