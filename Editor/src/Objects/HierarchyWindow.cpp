//
// Created by ounol on 2023-08-26.
//

#include "HierarchyWindow.h"
#include "../Manager/EEngineCore.h"
#include "../Manager/EditorActionLogger.h"
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
    if (ImGui::BeginMenu("Create")) {
        if (ImGui::MenuItem("Empty Object")) {
            auto* obj = CreateEmptyGameObject(parent);
            m_selected = obj;
            m_core->InvokeEditorRender();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("3D Object")) {
            if (ImGui::MenuItem("Cube")) {
                auto* obj = CreatePrimitiveGameObject("Cube", parent);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            if (ImGui::MenuItem("Sphere")) {
                auto* obj = CreatePrimitiveGameObject("Sphere", parent);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            if (ImGui::MenuItem("Plane")) {
                auto* obj = CreatePrimitiveGameObject("Plane", parent);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light")) {
            if (ImGui::MenuItem("Directional Light")) {
                auto* obj = CreateEmptyGameObject(parent);
                obj->SetName("Directional Light");
                auto* light = obj->CreateComponent<CSE::LightComponent>();
                light->SetLightType(CSE::LightComponent::DIRECTIONAL);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            if (ImGui::MenuItem("Point Light")) {
                auto* obj = CreateEmptyGameObject(parent);
                obj->SetName("Point Light");
                auto* light = obj->CreateComponent<CSE::LightComponent>();
                light->SetLightType(CSE::LightComponent::POINT);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            if (ImGui::MenuItem("Spot Light")) {
                auto* obj = CreateEmptyGameObject(parent);
                obj->SetName("Spot Light");
                auto* light = obj->CreateComponent<CSE::LightComponent>();
                light->SetLightType(CSE::LightComponent::SPOT);
                m_selected = obj;
                m_core->InvokeEditorRender();
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Camera")) {
            auto* obj = CreateEmptyGameObject(parent);
            obj->SetName("Camera");
            obj->CreateComponent<CSE::CameraComponent>();
            m_selected = obj;
            m_core->InvokeEditorRender();
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
    const auto& sceneMgr = CORE->GetSceneMgrCore();
    if (sceneMgr == nullptr) return nullptr;

    const auto& scene = dynamic_cast<CSE::SScene*>(sceneMgr->GetCurrentScene());
    if (scene == nullptr) return nullptr;

    auto* newObj = new CSE::SGameObject("New GameObject");
    newObj->CreateComponent<CSE::TransformComponent>();

    std::string parentName = parent != nullptr ? parent->GetName() : "Root";
    if (parent != nullptr) {
        parent->AddChild(newObj);
    } else {
        scene->GetRoot()->AddChild(newObj);
    }

    newObj->Init();
    UpdateGameObject(*newObj);

    ACTION_LOG_GAMEOBJECT("Created empty GameObject", newObj->GetName(), "Parent: " + parentName);

    return newObj;
}

CSE::SGameObject* HierarchyWindow::CreatePrimitiveGameObject(const char* name, CSE::SGameObject* parent) {
    auto* obj = CreateEmptyGameObject(parent);
    if (obj == nullptr) return nullptr;

    obj->SetName(name);

    // Add RenderComponent
    auto* renderComp = obj->CreateComponent<CSE::RenderComponent>();

    // Set default mesh based on primitive type
    // Note: Mesh resources should be loaded from assets
    // This sets up the component structure, actual mesh loading depends on available assets

    ACTION_LOG_GAMEOBJECT("Created primitive GameObject", name, "Type: 3D Object");

    return obj;
}

void HierarchyWindow::DeleteSelectedGameObject() {
    if (m_selected == nullptr) return;

    // Don't delete root object
    if (m_selected->GetParent() == nullptr) return;

    std::string deletedName = m_selected->GetName();
    std::string parentName = m_selected->GetParent() != nullptr ? m_selected->GetParent()->GetName() : "Root";

    auto* parent = m_selected->GetParent();
    if (parent != nullptr) {
        parent->RemoveChild(m_selected);
    }

    m_selected->Destroy();
    m_selected = nullptr;

    ACTION_LOG_GAMEOBJECT("Deleted GameObject", deletedName, "Parent was: " + parentName);

    m_core->InvokeEditorRender();
}

void HierarchyWindow::DuplicateSelectedGameObject() {
    if (m_selected == nullptr) return;

    // Get parent for the duplicated object
    auto* parent = m_selected->GetParent();
    if (parent == nullptr) return;

    std::string sourceName = m_selected->GetName();

    // Create a simple duplicate (deep copy would require more implementation)
    auto* newObj = new CSE::SGameObject(m_selected->GetName() + " (Copy)");

    // Copy transform
    auto* srcTransform = static_cast<CSE::TransformComponent*>(m_selected->GetTransform());
    auto* newTransform = newObj->CreateComponent<CSE::TransformComponent>();
    if (srcTransform != nullptr && newTransform != nullptr) {
        newTransform->SetMatrix(srcTransform->GetMatrix());
    }

    // Copy other components (basic implementation)
    for (const auto& comp : m_selected->GetComponents()) {
        if (comp->IsSameClass("TransformComponent")) continue;

        auto* clonedComp = comp->Clone(newObj);
        if (clonedComp != nullptr) {
            newObj->AddComponent(clonedComp);
        }
    }

    parent->AddChild(newObj);
    newObj->Init();
    UpdateGameObject(*newObj);

    ACTION_LOG_GAMEOBJECT("Duplicated GameObject", newObj->GetName(), "Source: " + sourceName);

    m_selected = newObj;
    m_core->InvokeEditorRender();
}
