#include "InspectorWindow.h"
#include "Base/HierarchyData.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Object/SGameObject.h"
#include "../../src/Component/SComponent.h"
#include "../../src/Util/Loader/XML/XML.h"
#include "../../src/Util/Render/SMaterial.h"
#include "../../src/Component/RenderComponent.h"
#include <regex>

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

    const auto& selected = m_core->GetHierarchyData()->GetSelectedObject();
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
    }

    for (const auto& layer : m_layers) {
        layer->UpdateParams();
        layer->RenderUI();
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
