#include "MaterialLayer.h"
#include "InspectorParam.h"
#include "../../../src/MacroDef.h"
#include "../../../src/Util/Render/SMaterial.h"
#include "../../../src/Util/Render/STexture.h"
#include "../../../src/Util/Loader/XML/XML.h"


using namespace CSEditor;

MaterialLayer::MaterialLayer(CSE::SMaterial& material) {
    m_material = &material;
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

}

void MaterialLayer::InitParams() {

    for(const auto& element_pair : m_material->GetElements()) {
        const auto& element = element_pair.second;
        const CSE::SType& type = element->type;
        XNode* delivery = new XNode();
        delivery->name = element_pair.first;

        delivery->attributes.emplace_back("t", ConvertSTypeToParamType(type));
        delivery->attributes.emplace_back("d", ConvertSTypeToClassType(type));
        delivery->value = "";
        for (const auto& str : element->valueStr) {
            delivery->value += str + ' ';
        }
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
