#include "SMaterial.h"

#include "../Loader/XML/XML.h"
#include "../Loader/XML/XMLParser.h"
#include "ShaderUtil.h"
#include "../../Manager/LightMgr.h"
#include "../Settings.h"

using namespace CSE;

SMaterial::SMaterial() {
    m_lightMgr = CORE->GetCore(LightMgr);
}

SMaterial::SMaterial(const SMaterial* material) : SResource(material, false) {

    if (material == nullptr) throw -1;

    m_refHash = material->m_hash;

    for (const auto& element_pair : material->m_elements) {
        const auto& element_src = element_pair.second;
        auto element_copy = new Element;
        element_copy->count = element_src->count;
        element_copy->type = element_src->type;
        element_copy->valueStr = element_src->valueStr;
        m_elements.insert(std::pair<std::string, Element*>(element_pair.first, element_copy));
    }
    m_mode = material->m_mode;
    m_shaders = material->m_shaders;
    m_orderLayer = material->m_orderLayer;
    m_textureLayout = material->m_textureLayout;

    InitElements(m_elements, m_shaders);
    m_lightMgr = CORE->GetCore(LightMgr);
}

SMaterial::~SMaterial() {
    ReleaseElements();
}

void SMaterial::Exterminate() {
    ReleaseElements();
}

void SMaterial::ReleaseElements() {
    for (const auto& pair : m_elements) {
        auto* element = pair.second;
        SAFE_DELETE(element);
    }
    m_elements.clear();
}

void SMaterial::AttachElement() const {
    m_textureLayout = m_lightMgr->GetShadowCount();

    for (const auto& element_pair : m_elements) {
        const auto& element = element_pair.second;
        if (element->id < 0) continue;
        element->attachFunc();
    }
}

void SMaterial::InitElements(const ElementsMap& elements, SShaderGroup* shaders) {
    const auto& handle = shaders->GetHandleByMode(m_mode);
    for (const auto& element_pair : elements) {
        const auto& element_name = element_pair.first.c_str();
        const auto& element = element_pair.second;
        if (element->attachFunc != nullptr) continue;

        const auto& handleElement = handle->UniformLocation(element_name);
        if (handleElement == nullptr) continue;

        element->id = handleElement->id;
        SetBindFuncByType(element);
    }
//    std::string str = PrintMaterial();
//    std::string path = GetAssetReference(m_refHash)->path;
//    SaveTxtFile(path, str);
}

void SMaterial::SetInt(const std::string& name, int value) {
    auto find_iter = m_elements.find(name);
    if (find_iter == m_elements.end()) return;
    SetIntFunc(find_iter->second, value);
}

void SMaterial::SetFloat(const std::string& name, float value) {
    auto find_iter = m_elements.find(name);
    if (find_iter == m_elements.end()) return;
    SetFloatFunc(find_iter->second, value);
}

void SMaterial::SetVec3(const std::string& name, const vec3& value) {
    auto find_iter = m_elements.find(name);
    if (find_iter == m_elements.end()) return;
    SetVec3Func(find_iter->second, value);
}

void SMaterial::SetTexture(const std::string& name, SResource* texture) {
    auto find_iter = m_elements.find(name);
    if (find_iter == m_elements.end()) return;
    SetTextureFunc(find_iter->second, texture);
}

void SMaterial::Init(const AssetMgr::AssetReference* asset) {
    const XNode* root;

    try {
        root = XFILE(asset->name_path.c_str()).getRoot();
    }
    catch (int e) {
        return;
    }

    XNode cse_mat = root->getChild("CSEMAT");
    XNode shader_node = cse_mat.getChild("shader");

    auto var_nodes = shader_node.children;
    auto shader_file_id = shader_node.getAttribute("id").value;
    auto shaderGroup = Create<SShaderGroup>(shader_file_id);
    if (shaderGroup == nullptr) return;
    m_shaders = shaderGroup;

    if (shader_node.hasAttribute("deferred")) {
        auto get_deferred = std::stoi(shader_node.getAttribute("deferred").value);
        if (get_deferred == 1) {
            m_mode = DEFERRED;
        }
    }

    for (const auto& node : var_nodes) {

        auto element_value = node.value.toStringVector();
        auto element_type = node.getAttribute("type").value;
        SType type = XMLParser::GetType(element_type);
        auto element_name = node.getAttribute("name").value;
        auto element_count = node.getAttribute("count").value;

        auto element = new Element;
        element->type = type;
        element->count = std::stoi(element_count);
        element->valueStr = element_value;
        m_elements.insert(std::pair<std::string, Element*>(element_name, element));
    }
    SAFE_DELETE(root);
}

void SMaterial::SetBindFuncByType(Element* element) {
    const GLenum type = element->type;
    switch (type) {
        case FLOAT:
            SetFloatFunc(element, XMLParser::parseFloat(element->valueStr[0].c_str()));
            break;
        case INT:
            SetIntFunc(element, XMLParser::parseInt(element->valueStr[0].c_str()));
            break;
            //	case SType::MAT4:
            //		SetMat4Func(element, XMLParser::parseMat4(element->valueStr));
            //            break;
            //	case SType::MAT3:
            //		SetMat3Func(element, XMLParser::parseMat3(element->valueStr));
            //            break;
        case VEC4:
            SetVec4Func(element, XMLParser::parseVec4(element->valueStr));
            break;
        case VEC3:
            SetVec3Func(element, XMLParser::parseVec3(element->valueStr));
            break;
        case TEXTURE:
            SetTextureFunc(element, XMLParser::parseTexture(element->valueStr[0].c_str()));
            break;
    }
}

void SMaterial::SetIntFunc(Element* element, int value) {
    if (element == nullptr) return;
    element->raw = std::to_string(value);
    element->attachFunc = [element, value]() {
        glUniform1i(element->id, value);
    };
}

void SMaterial::SetFloatFunc(Element* element, float value) {
    if (element == nullptr) return;
    element->raw = std::to_string(value);
    element->attachFunc = [element, value]() {
        glUniform1f(element->id, value);
    };
}

void SMaterial::SetBoolFunc(Element* element, bool value) {
    if (element == nullptr) return;
    element->raw = std::to_string(value);
    element->attachFunc = [element, value]() {
        glUniform1i(element->id, value);
    };
}

void SMaterial::SetMat4Func(Element* element, mat4 value) {
    if (element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix4fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetMat3Func(Element* element, mat3 value) {
    if (element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix3fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetMat2Func(Element* element, mat2 value) {
    if (element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix2fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetVec4Func(Element* element, vec4 value) {
    element->attachFunc = [element, value]() {
        element->raw = std::to_string(value.x) + " "
                       + std::to_string(value.y) + " "
                       + std::to_string(value.z) + " "
                       + std::to_string(value.w);
        glUniform4fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetVec3Func(Element* element, vec3 value) {
    if (element == nullptr) return;
    element->raw = std::to_string(value.x) + " "
                   + std::to_string(value.y) + " "
                   + std::to_string(value.z);
    element->attachFunc = [element, value]() {
        glUniform3fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetVec2Func(Element* element, vec2 value) {
    if (element == nullptr) return;
    element->raw = std::to_string(value.x) + " "
                   + std::to_string(value.y);
    element->attachFunc = [element, value]() {
        glUniform2fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetTextureFunc(Element* element, SResource* texture) {
    if (element == nullptr || texture == nullptr) return;
    auto value = static_cast<STexture*>(texture);
    //    element->count = m_textureLayout++;
    auto* texture_layout = &m_textureLayout;
    ++m_textureCount;
    element->raw = texture->GetHash();
    element->attachFunc = [element, value, texture_layout]() {
        value->Bind(element->id, *texture_layout);
        ++(*texture_layout);
    };
}

short SMaterial::GetOrderLayer() const {
    return m_orderLayer;
}

void SMaterial::SetOrderLayer(int orderLayer) {
    m_orderLayer = orderLayer;
}

SShaderGroup* SMaterial::GetShaders() const {
    return m_shaders;
}

SMaterial::SMaterialMode SMaterial::GetMode() const {
    return m_mode;
}

void SMaterial::SetMode(SMaterialMode mode) {
    m_mode = mode;
}

int SMaterial::GetTextureCount() const {
    return m_textureLayout;
}

SMaterial* SMaterial::GenerateMaterial(SShaderGroup* shaders) {
    if (shaders == nullptr) return nullptr;
    const auto& handle = shaders->GetHandleByMode(SMaterialMode::NORMAL);

    const auto& uniformList = handle->GetUniformsList();
    auto material = new SMaterial();
    material->m_shaders = shaders;

    for (const auto& uniform_pair : uniformList) {
        const auto& name = uniform_pair.first;
        const auto& gl_element = uniform_pair.second;

        auto element = new Element;
        element->id = gl_element->id;
        element->type = XMLParser::GetType(gl_element->type);

        material->m_elements.insert(std::pair<std::string, Element*>(name, element));
    }
    material->InitElements(material->m_elements, shaders);

    return material;
}

std::string SMaterial::PrintMaterial() const {
    std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                         "<CSEMAT version=\"1.0.0\">\n";

    if(m_mode == DEFERRED) {
        result += "<shader id=\"" + m_shaders->GetHash() + "\" deferred=\"1\">\n";
    }
    else {
        result += "<shader id=\"" + m_shaders->GetHash() + "\">\n";
    }

    for (const auto& var : m_elements) {
        const auto& element = var.second;
        if(element->raw.empty()) continue;

        result += "<var name=\"" + var.first
                  + "\" type=\"" + XMLParser::ToString(element->type)
                  + "\" count=\"1\">"
                  + element->raw + "</var>\n";
    }
    result += "</shader>\n</CSEMAT>";
    return result;
}
