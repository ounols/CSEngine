//
// Created by ounol on 2023-08-26.
//

#include "HierarchyWindow.h"
#include "../Manager/EEngineCore.h"
#include "../Manager/EditorActionLogger.h"
#include "../Backend/ObjectBackend.h"
#include "../Backend/EditorBackend.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Manager/GameObjectMgr.h"
#include "../../src/Object/SScene.h"
#include "../../src/Object/SPrefab.h"
#include "../../../src/Manager/ResMgr.h"
#include "../../src/Component/TransformComponent.h"
#include "../../src/Component/RenderComponent.h"
#include "../../src/Component/CameraComponent.h"
#include "../../src/Component/LightComponent.h"

using namespace CSEditor;

HierarchyWindow::HierarchyWindow() {
    m_core = EEngineCore::getEditorInstance();
    m_core->SetHierarchyData(this);
}

HierarchyWindow::~HierarchyWindow() = default;

void HierarchyWindow::SetUI() {
    ImGui::Begin("Hierarchy");

    RenderTrees();

    // Context menu for empty space
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        RenderCreateMenu(nullptr);
        ImGui::EndPopup();
    }

    // Handle keyboard shortcuts
    if (!m_core->IsPreview() && ImGui::IsWindowFocused()) {
        HandleKeyboardShortcuts();
    }

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
        if (m_selected != &parent) {
            ACTION_LOG_SELECTION(parent.GetName(), true);
        }
        m_selected = &parent;
    }

    // Context menu for this item
    if (ImGui::BeginPopupContextItem()) {
        m_selected = &parent;

        RenderCreateMenu(&parent);

        ImGui::Separator();

        if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
            DuplicateSelectedGameObject();
        }
        if (ImGui::MenuItem("Delete", "Delete")) {
            DeleteSelectedGameObject();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Rename")) {
            // TODO: Implement rename functionality
        }

        ImGui::EndPopup();
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

void HierarchyWindow::RenderContextMenu() {
    // This is now handled in RenderGameObject and SetUI
}

void HierarchyWindow::RenderCreateMenu(CSE::SGameObject* parent) {
    auto& objBackend = ObjectBackend::GetInstance();
    auto& editorBackend = EditorBackend::GetInstance();

    if (ImGui::BeginMenu("Create")) {
        if (ImGui::MenuItem("Empty Object")) {
            auto* obj = CreateEmptyGameObject(parent);
            m_selected = obj;
            editorBackend.InvokeEditorRender();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("3D Object")) {
            if (ImGui::MenuItem("Cube")) {
                auto* obj = CreatePrimitiveGameObject("Cube", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            if (ImGui::MenuItem("Sphere")) {
                auto* obj = CreatePrimitiveGameObject("Sphere", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            if (ImGui::MenuItem("Plane")) {
                auto* obj = CreatePrimitiveGameObject("Plane", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light")) {
            if (ImGui::MenuItem("Directional Light")) {
                auto* obj = objBackend.CreateLightObjectDirect("Directional", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            if (ImGui::MenuItem("Point Light")) {
                auto* obj = objBackend.CreateLightObjectDirect("Point", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            if (ImGui::MenuItem("Spot Light")) {
                auto* obj = objBackend.CreateLightObjectDirect("Spot", parent);
                m_selected = obj;
                editorBackend.InvokeEditorRender();
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Camera")) {
            auto* obj = objBackend.CreateCameraObjectDirect(parent);
            m_selected = obj;
            editorBackend.InvokeEditorRender();
        }

        ImGui::EndMenu();
    }
}

void HierarchyWindow::HandleKeyboardShortcuts() {
    // Delete key - delete selected object
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && m_selected != nullptr) {
        DeleteSelectedGameObject();
    }

    // Ctrl+D - duplicate selected object
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D) && m_selected != nullptr) {
        DuplicateSelectedGameObject();
    }
}

CSE::SGameObject* HierarchyWindow::CreateEmptyGameObject(CSE::SGameObject* parent) {
    auto& backend = ObjectBackend::GetInstance();
    return backend.CreateEmptyObjectDirect("New GameObject", parent);
}

CSE::SGameObject* HierarchyWindow::CreatePrimitiveGameObject(const char* name, CSE::SGameObject* parent) {
    auto& backend = ObjectBackend::GetInstance();
    return backend.CreatePrimitiveObjectDirect(name, name, parent);
}

void HierarchyWindow::DeleteSelectedGameObject() {
    if (m_selected == nullptr) return;

    auto& backend = ObjectBackend::GetInstance();
    if (backend.DeleteObjectDirect(m_selected)) {
        m_selected = nullptr;
        EditorBackend::GetInstance().InvokeEditorRender();
    }
}

void HierarchyWindow::DuplicateSelectedGameObject() {
    if (m_selected == nullptr) return;

    auto& backend = ObjectBackend::GetInstance();
    auto* newObj = backend.DuplicateObjectDirect(m_selected);
    if (newObj) {
        m_selected = newObj;
        EditorBackend::GetInstance().InvokeEditorRender();
    }
}
