#include "InspectorLayer.h"
#include "InspectorParam.h"
#include "../../../src/Component/SComponent.h"
#include "../../../src/Util/Loader/XML/XML.h"
#include "../../../src/MacroDef.h"

#include "imgui.h"

using namespace CSEditor;

InspectorLayer::InspectorLayer(CSE::SComponent& component) {
    m_component = &component;
    InitParams();
}

InspectorLayer::~InspectorLayer() {
    for (auto* param: m_params) {
        SAFE_DELETE(param);
    }
    m_params.clear();
}

void InspectorLayer::UpdateParams() {
    if (m_params.empty()) return;
    const XNode* root = XFILE().loadBuffer(m_component->PrintValue());
    const auto& componentNode = root->getChild("component").children;
    const auto& size = m_params.size();
    for (int i = 0; i < size; ++i) {
        const auto& param = m_params[i];
        const auto& node = componentNode[i];
        param->UpdateParam(node);
    }
    delete root;
}

void InspectorLayer::RenderUI() {
    if (!ImGui::CollapsingHeader(m_component->GetClassType().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::BeginTable(m_component->GetHash().c_str(), 2, ImGuiTableFlags_None);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Enable");
    ImGui::TableNextColumn();
    bool enable = m_component->GetIsEnable();
    ImGui::Checkbox("", &enable);

    for (const auto& param: m_params) {
        const auto& name = param->GetName();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (param->PrintUI()) {
            const auto& values = param->GetParam();
            m_component->SetValue(name, values);
        }
        ImGui::PopItemWidth();
    }
    ImGui::EndTable();
}

void InspectorLayer::InitParams() {
    const auto buffer = m_component->PrintValue();
    if (buffer.empty()) return;

    const XNode* root = XFILE().loadBuffer(buffer);
    const auto& componentNode = root->getChild("component");
    m_params.reserve(componentNode.children.size());
    for (const auto& valueNode: componentNode.children) {
        m_params.push_back(new InspectorParam(valueNode));
    }

    delete root;
}