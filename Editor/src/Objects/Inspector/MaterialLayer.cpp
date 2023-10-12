#include "MaterialLayer.h"
#include "InspectorParam.h"
#include "../../../src/MacroDef.h"
#include "../../../src/Component/RenderComponent.h"
#include "../../../src/Util/Render/SMaterial.h"
#include "../../../src/Util/Render/STexture.h"
#include "../../../src/Util/Loader/XML/XML.h"
#include "../../../src/Util/MoreString.h"

#include "imgui.h"

using namespace CSEditor;

MaterialLayer::MaterialLayer(CSE::RenderComponent& component) {
    m_render = &component;
    m_material = component.GetMaterial();
    m_material_ref = component.GetMaterialReference();
    MaterialLayer::InitParams();
}

MaterialLayer::~MaterialLayer() {
    for (auto* param: m_params) {
        SAFE_DELETE(param);
    }
    m_params.clear();
}

void MaterialLayer::UpdateParams() {

}

void MaterialLayer::RenderUI() {
    if(m_render->GetMaterialReference() != m_material_ref) {
        m_material = m_render->GetMaterial();
        m_material_ref = m_render->GetMaterialReference();
        MaterialLayer::InitParams();
    }

    if (!ImGui::CollapsingHeader(m_material->GetClassType(), ImGuiTreeNodeFlags_DefaultOpen))
        return;
    // When Dragging
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("INSP_RES", m_material, sizeof(CSE::SResource));
        ImGui::EndDragDropSource();
    }

    ImGui::BeginTable(m_material->GetHash().c_str(), 2, ImGuiTableFlags_None);

    for (const auto& param: m_params) {
        const auto& name = param->GetName();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (param->PrintUI()) {
            const auto& values = param->GetParam();
            m_material->SetRawData(name, values);
        }
        ImGui::PopItemWidth();
    }
    ImGui::EndTable();
}

void MaterialLayer::InitParams() {
    for(const auto& element_pair : m_material->GetElements()) {
        const auto& element = element_pair.second;
        const CSE::SType& type = element->type;
        XNode* delivery = new XNode();
        delivery->name = "material";

        delivery->attributes.emplace_back("t", ConvertSTypeToParamType(type));
        delivery->attributes.emplace_back("d", ConvertSTypeToClassType(type));
        delivery->attributes.emplace_back("name", element_pair.first.c_str());
        delivery->value = "";
        for (const auto& str : element->valueStr) {
            delivery->value += str + ' ';
        }
        delivery->value = delivery->value.substr(0, delivery->value.size() - 1);
        m_params.push_back(new InspectorParam(*delivery));
        SAFE_DELETE(delivery);
    }
}

const char* MaterialLayer::ConvertSTypeToParamType(CSE::SType type) {
    switch (type) {
        case CSE::STRING:
            return "str";
        case CSE::BOOL:
            return "bool";
        case CSE::FLOAT:
            return "float";
        case CSE::INT:
            return "int";
        case CSE::VEC2:
            return "vec2";
        case CSE::VEC3:
            return "vec3";
        case CSE::VEC4:
        case CSE::QUATERNION:
            return "vec4";
        case CSE::RESOURCE:
        case CSE::TEXTURE:
        case CSE::MATERIAL:
            return "res";
        case CSE::COMPONENT:
            return "comp";
        case CSE::GAME_OBJECT:
            return "obj";
        default:
            return "";
    }
}

const char* MaterialLayer::ConvertSTypeToClassType(CSE::SType type) {
    switch (type) {
        case CSE::TEXTURE:
            return CSE::STexture::GetClassStaticType();
        case CSE::MATERIAL:
            return CSE::STexture::GetClassStaticType();
        default:
            return "";
    }
}
