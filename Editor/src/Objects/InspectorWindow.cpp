#include "InspectorWindow.h"
#include "Base/HierarchyData.h"
#include "../Manager/EEngineCore.h"
#include "../Manager/EditorActionLogger.h"
#include "../Backend/ComponentBackend.h"
#include "../Backend/EditorBackend.h"
#include "../../src/Object/SGameObject.h"
#include "../../src/Component/SComponent.h"
#include "../../src/Component/TransformComponent.h"
#include "../../src/Component/RenderComponent.h"
#include "../../src/Component/CameraComponent.h"
#include "../../src/Component/LightComponent.h"
#include "../../src/Component/CustomComponent.h"
#include "../../src/Component/Animation/AnimatorComponent.h"
#include "../../src/Util/Loader/XML/XML.h"
#include "../../src/Util/Render/SMaterial.h"
#include <regex>
#include <cstring>

#include "Inspector/InspectorLayer.h"
#include "Inspector/MaterialLayer.h"

using namespace CSEditor;
using namespace CSE;

InspectorWindow::InspectorWindow() {
    m_core = EEngineCore::getEditorInstance();
}

InspectorWindow::~InspectorWindow() {
    ReleaseLayers();
}

void InspectorWindow::SetUI() {
    ImGui::Begin("Inspector");

    auto* selected = m_core->GetHierarchyData()->GetSelectedObject();
    if (selected == nullptr) {
        if(!m_layers.empty()) ReleaseLayers();
        ImGui::End();
        return;
    }

    if(!m_core->IsPreview() && (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())) {
        for (ImGuiKey key = static_cast<ImGuiKey>(0); key < ImGuiKey_COUNT; key = (ImGuiKey) (key + 1)) {
            if (ImGui::IsKeyDown(key)) {
                m_core->InvokeEditorRender();
                break;
            }
        }
    }

    const bool isChange = selected != m_selectedPrev;
    if (isChange) {
        ReleaseLayers();
        InitLayers(*selected);
        // Update name buffer
        strncpy(m_nameBuffer, selected->GetName().c_str(), sizeof(m_nameBuffer) - 1);
    }

    // Render GameObject header (name, active state)
    RenderGameObjectHeader(selected);

    ImGui::Separator();

    // Render component layers
    for (const auto& layer : m_layers) {
        layer->UpdateParams();
        layer->RenderUI();
    }

    ImGui::Separator();

    // Add Component button
    if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
        m_showAddComponentPopup = true;
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        RenderAddComponentMenu(selected);
        ImGui::EndPopup();
    }

    ImGui::End();
    m_selectedPrev = selected;
}

void InspectorWindow::ReleaseLayers() {
    for (auto* layer: m_layers) {
        SAFE_DELETE(layer);
    }
    m_layers.clear();
}

void InspectorWindow::InitLayers(const CSE::SGameObject& object) {
    const auto& components = object.GetComponents();
    RenderComponent* selected_render = nullptr;
    const auto& render_ref = ReflectionRef<RenderComponent>();
    m_layers.reserve(components.size());
    for (const auto& component: components) {
        m_layers.push_back(new InspectorLayer(*component));
        if(render_ref.IsSameClass(component)) {
            selected_render = static_cast<RenderComponent*>(component);
        }
    }

    if(selected_render != nullptr) {
        m_layers.push_back(new MaterialLayer(*selected_render));
    }
}

void InspectorWindow::RenderGameObjectHeader(CSE::SGameObject* object) {
    if (object == nullptr) return;

    // Active checkbox
    bool isActive = object->GetIsEnable();
    if (ImGui::Checkbox("##Active", &isActive)) {
        object->SetIsEnable(isActive);
        ACTION_LOG_INSPECTOR("Active state changed", object->GetName(), "Active", isActive ? "true" : "false");
        m_core->InvokeEditorRender();
    }

    ImGui::SameLine();

    // Name input field
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##Name", m_nameBuffer, sizeof(m_nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::string oldName = object->GetName();
        object->SetName(m_nameBuffer);
        ACTION_LOG_GAMEOBJECT("Renamed GameObject", m_nameBuffer, "Previous name: " + oldName);
        m_core->InvokeEditorRender();
    }
    ImGui::PopItemWidth();

    // Show object info
    ImGui::TextDisabled("Hash: %s", object->GetHash().c_str());

    // Static/Prefab tag
    if (object->isPrefab()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "[Prefab]");
    }
}

void InspectorWindow::RenderAddComponentMenu(CSE::SGameObject* object) {
    if (object == nullptr) return;

    auto& compBackend = ComponentBackend::GetInstance();
    auto& editorBackend = EditorBackend::GetInstance();

    ImGui::Text("Add Component");
    ImGui::Separator();

    // Check which components already exist using backend
    bool hasRender = compBackend.HasComponent(object, "RenderComponent");
    bool hasCamera = compBackend.HasComponent(object, "CameraComponent");
    bool hasLight = compBackend.HasComponent(object, "LightComponent");
    bool hasAnimator = compBackend.HasComponent(object, "AnimatorComponent");

    if (ImGui::BeginMenu("Rendering")) {
        if (ImGui::MenuItem("Render Component", nullptr, false, !hasRender)) {
            compBackend.AddComponentDirect(object, "RenderComponent");
            ReleaseLayers();
            InitLayers(*object);
            editorBackend.InvokeEditorRender();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Camera")) {
        if (ImGui::MenuItem("Camera Component", nullptr, false, !hasCamera)) {
            compBackend.AddComponentDirect(object, "CameraComponent");
            ReleaseLayers();
            InitLayers(*object);
            editorBackend.InvokeEditorRender();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Lighting")) {
        if (ImGui::MenuItem("Light Component", nullptr, false, !hasLight)) {
            compBackend.AddComponentDirect(object, "LightComponent");
            ReleaseLayers();
            InitLayers(*object);
            editorBackend.InvokeEditorRender();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Animation")) {
        if (ImGui::MenuItem("Animator Component", nullptr, false, !hasAnimator)) {
            compBackend.AddComponentDirect(object, "AnimatorComponent");
            ReleaseLayers();
            InitLayers(*object);
            editorBackend.InvokeEditorRender();
        }
        ImGui::EndMenu();
    }

    // Script component is special - can have multiple
    if (ImGui::BeginMenu("Scripts")) {
        if (ImGui::MenuItem("Custom Script")) {
            compBackend.AddComponentDirect(object, "CustomComponent");
            ReleaseLayers();
            InitLayers(*object);
            editorBackend.InvokeEditorRender();
        }
        ImGui::EndMenu();
    }
}

void InspectorWindow::RemoveComponent(CSE::SGameObject* object, CSE::SComponent* component) {
    if (object == nullptr || component == nullptr) return;

    auto& compBackend = ComponentBackend::GetInstance();
    if (compBackend.RemoveComponentDirect(object, component)) {
        ReleaseLayers();
        InitLayers(*object);
        EditorBackend::GetInstance().InvokeEditorRender();
    }
}
