#include <unordered_map>
#include "InspectorParam.h"
#include "../../Manager/EEngineCore.h"
#include "../../../src/Util/Loader/XML/XML.h"
#include "../../../src/Util/MoreString.h"
#include "../../../src/Util/Vector.h"
#include "../../../src/MacroDef.h"
#include "../../../src/Manager/ResMgr.h"
#include "../../../src/Manager/GameObjectMgr.h"
#include "imgui.h"

using namespace CSEditor;

namespace __CSEditor_Inspector__ {
    const std::unordered_map<std::string, InspectorParamType> typeMap = {
            { "int",   InspectorParamType::INT },
            { "float", InspectorParamType::FLOAT },
            { "vec2",  InspectorParamType::VEC2 },
            { "vec3",  InspectorParamType::VEC3 },
            { "vec4",  InspectorParamType::VEC4 },
            { "str",   InspectorParamType::STRING },
            { "bool",  InspectorParamType::BOOL },
            { "res",   InspectorParamType::RES },
            { "obj",   InspectorParamType::OBJ },
            { "comp",  InspectorParamType::COMP },
            { "col3",  InspectorParamType::COLOR3 },
            { "col4",  InspectorParamType::COLOR4 },
    };
}
using namespace __CSEditor_Inspector__;

InspectorParam::InspectorParam(const XNode& node) {
    m_name = node.getAttribute("name").value;

    const auto& type = (node.hasAttribute("t")) ? node.getAttribute("t").value : "";
    const auto& type_iter = typeMap.find(type);
    if (type_iter == typeMap.end())
        m_type = InspectorParamType::UNKNOWN;
    else
        m_type = type_iter->second;

    GenerateValue(node);
    GenerateFunc();
}

InspectorParam::~InspectorParam() {
    if (m_type == InspectorParamType::UNKNOWN) return;
    m_deleteFunc();
}

bool InspectorParam::PrintUI() {
    if (m_type == InspectorParamType::UNKNOWN) return false;
    return m_paramFunc();
}

std::vector<std::string> InspectorParam::GetParam() {
    if (m_type == InspectorParamType::UNKNOWN) return std::vector<std::string>();
    return m_getFunc();
}

void InspectorParam::UpdateParam(const XNode& node) {
    if (m_type == InspectorParamType::UNKNOWN) return;
    m_updateFunc(node);
}

void InspectorParam::GenerateValue(const XNode& node) {
    const auto& values = node.value.toStringVector();
    switch (m_type) {
        case InspectorParamType::INT:
        case InspectorParamType::BOOL: {
            const auto& v = std::stoi(values[0]);
            m_value = new int{ v };
            auto* ptr = static_cast<int*>(m_value);
            m_deleteFunc = [ptr]() {
                delete ptr;
            };
            m_updateFunc = [ptr](const XNode& node) {
                const auto& values = node.value.toStringVector();
                *ptr = std::stoi(values[0]);
            };
            break;
        }
        case InspectorParamType::FLOAT: {
            const auto& v = std::stof(values[0]);
            m_value = new float{ v };
            auto* ptr = static_cast<float*>(m_value);
            m_deleteFunc = [ptr]() {
                delete ptr;
            };
            m_updateFunc = [ptr](const XNode& node) {
                const auto& values = node.value.toStringVector();
                *ptr = std::stof(values[0]);
            };
            break;
        }
        case InspectorParamType::VEC2: {
            const auto& v1 = std::stof(values[0]);
            const auto& v2 = std::stof(values[1]);
            m_value = new CSE::vec2{ v1, v2 };
            auto* ptr = static_cast<CSE::vec2*>(m_value);
            m_deleteFunc = [ptr]() {
                delete ptr;
            };
            m_updateFunc = [ptr](const XNode& node) {
                const auto& values = node.value.toStringVector();
                ptr->x = std::stof(values[0]);
                ptr->y = std::stof(values[1]);
            };
            break;
        }
        case InspectorParamType::VEC3:
        case InspectorParamType::COLOR3: {
            const auto& v1 = std::stof(values[0]);
            const auto& v2 = std::stof(values[1]);
            const auto& v3 = std::stof(values[2]);
            m_value = new CSE::vec3{ v1, v2, v3 };
            auto* ptr = static_cast<CSE::vec3*>(m_value);
            m_deleteFunc = [ptr]() {
                delete ptr;
            };
            m_updateFunc = [ptr](const XNode& node) {
                const auto& values = node.value.toStringVector();
                ptr->x = std::stof(values[0]);
                ptr->y = std::stof(values[1]);
                ptr->z = std::stof(values[2]);
            };
            break;
        }
        case InspectorParamType::VEC4:
        case InspectorParamType::COLOR4: {
            const auto& v1 = std::stof(values[0]);
            const auto& v2 = std::stof(values[1]);
            const auto& v3 = std::stof(values[2]);
            const auto& v4 = std::stof(values[3]);
            m_value = new CSE::vec4{ v1, v2, v3, v4 };
            auto* ptr = static_cast<CSE::vec4*>(m_value);
            m_deleteFunc = [ptr]() {
                delete ptr;
            };
            m_updateFunc = [ptr](const XNode& node) {
                const auto& values = node.value.toStringVector();
                ptr->x = std::stof(values[0]);
                ptr->y = std::stof(values[1]);
                ptr->z = std::stof(values[2]);
                ptr->w = std::stof(values[3]);
            };
            break;
        }
        case InspectorParamType::COMP: {
            const auto& str = node.value.c_str();
            const auto& ptr = CORE->GetGameObjectMgrCore()->FindComponentByHash(str);
            const auto& type = node.getAttribute("d").value.c_str();
            m_classType = type;
            m_value = ptr;
            m_deleteFunc = []() {};
            m_updateFunc = [this](const XNode& node) {
                const auto& str = node.value.c_str();
                const auto& ptr = CORE->GetGameObjectMgrCore()->FindComponentByHash(str);
                this->m_value = ptr;
            };
            break;
        }
        case InspectorParamType::OBJ: {
            const auto& str = node.value.c_str();
            const auto& ptr = CORE->GetGameObjectMgrCore()->FindByHash(str);
            m_value = ptr;
            m_deleteFunc = []() {};
            m_updateFunc = [this](const XNode& node) {
                const auto& str = node.value.c_str();
                const auto& ptr = CORE->GetGameObjectMgrCore()->FindByHash(str);
                this->m_value = ptr;
            };
            break;
        }
        case InspectorParamType::RES: {
            const auto& str = node.value.c_str();
            const auto& ptr = CSE::SResource::Get<CSE::SResource>(str);
            const auto& type = node.getAttribute("d").value.c_str();
            m_classType = type;
            m_value = ptr;
            m_deleteFunc = []() {};
            m_updateFunc = [this](const XNode& node) {
                const auto& str = node.value.c_str();
                const auto& ptr = CSE::SResource::Create(str, this->m_classType);
                this->m_value = ptr;
            };
            break;
        }
        case InspectorParamType::STRING: {
            char* buf = new char[128]{ 0 };
            const auto& str = node.value.c_str();
            const int size = node.value.size();
            for (int i = 0; i < size; ++i) {
                buf[i] = str[i];
            }
            m_value = buf;
            auto* ptr = static_cast<char*>(m_value);
            m_deleteFunc = [ptr]() {
                delete[] ptr;
            };
            m_updateFunc = [this](const XNode& node) {
                ReplaceValueString(node.value.c_str(), node.value.size());
            };
            break;
        }
        default:
            break;
    }
}

void InspectorParam::GenerateFunc() {
    m_nameId = CSE::GetRandomHash(6);
    auto name = m_nameId.c_str();
    
    switch (m_type) {
        case InspectorParamType::INT:
            GenerateIntFunc(name);
            break;
        case InspectorParamType::FLOAT:
            GenerateFloatFunc(name);
            break;
        case InspectorParamType::VEC2:
            GenerateVec2Func(name);
            break;
        case InspectorParamType::VEC3:
            GenerateVec3Func(name);
            break;
        case InspectorParamType::COLOR3:
            GenerateColor3Func(name);
            break;
        case InspectorParamType::VEC4:
            GenerateVec4Func(name);
            break;
        case InspectorParamType::COLOR4:
            GenerateColor4Func(name);
            break;
        case InspectorParamType::STRING:
            GenerateStringFunc(name);
            break;
        case InspectorParamType::BOOL:
            GenerateBoolFunc(name);
            break;
        case InspectorParamType::RES:
            GenerateResourceFunc();
            break;
        case InspectorParamType::OBJ:
            GenerateObjectFunc();
            break;
        case InspectorParamType::COMP:
            GenerateComponentFunc();
            break;
        default:
            break;
    }
}

void InspectorParam::ReplaceValueString(const char* str, const int size) {
    auto* ptr = static_cast<char*>(m_value);
    memset(ptr, 0, sizeof(char) * 128);
    for (int i = 0; i < size; ++i) {
        ptr[i] = str[i];
    }
}

void InspectorParam::GenerateIntFunc(const char* name) {
    auto* value = static_cast<int*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::DragInt(name, value);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(1);
        result.emplace_back(std::to_string(*value));
        return result;
    };
}

void InspectorParam::GenerateFloatFunc(const char* name) {
    auto* value = static_cast<float*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::DragFloat(name, value, 0.005f);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(1);
        result.emplace_back(std::to_string(*value));
        return result;
    };
}

void InspectorParam::GenerateVec2Func(const char* name) {
    auto* value = static_cast<CSE::vec2*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::InputFloat2(name, const_cast<float*>(value->Pointer()));
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(2);
        result.emplace_back(std::to_string(value->x));
        result.emplace_back(std::to_string(value->y));
        return result;
    };
}

void InspectorParam::GenerateVec3Func(const char* name) {
    auto* value = static_cast<CSE::vec3*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::DragFloat3(name, const_cast<float*>(value->Pointer()), 0.005f);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(3);
        result.emplace_back(std::to_string(value->x));
        result.emplace_back(std::to_string(value->y));
        result.emplace_back(std::to_string(value->z));
        return result;
    };
}

void InspectorParam::GenerateColor3Func(const char* name) {
    auto* value = static_cast<CSE::vec3*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::ColorEdit3(name, const_cast<float*>(value->Pointer()),
                                 ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(3);
        result.emplace_back(std::to_string(value->x));
        result.emplace_back(std::to_string(value->y));
        result.emplace_back(std::to_string(value->z));
        return result;
    };
}

void InspectorParam::GenerateVec4Func(const char* name) {
    auto* value = static_cast<CSE::vec4*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::DragFloat4(name, const_cast<float*>(value->Pointer()), 0.005f);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(4);
        result.emplace_back(std::to_string(value->x));
        result.emplace_back(std::to_string(value->y));
        result.emplace_back(std::to_string(value->z));
        result.emplace_back(std::to_string(value->w));
        return result;
    };
}

void InspectorParam::GenerateColor4Func(const char* name) {
    auto* value = static_cast<CSE::vec4*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::ColorEdit4(name, const_cast<float*>(value->Pointer()),
                                 ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(4);
        result.emplace_back(std::to_string(value->x));
        result.emplace_back(std::to_string(value->y));
        result.emplace_back(std::to_string(value->z));
        result.emplace_back(std::to_string(value->w));
        return result;
    };
}

void InspectorParam::GenerateStringFunc(const char* name) {
    auto* value = static_cast<char*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::InputText(name, value, 128);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(1);
        result.emplace_back(value);
        return result;
    };
}

void InspectorParam::GenerateBoolFunc(const char* name) {
    auto* value = static_cast<bool*>(m_value);
    m_paramFunc = [value, name]() {
        return ImGui::Checkbox(name, value);
    };
    m_getFunc = [value]() {
        std::vector<std::string> result;
        result.reserve(1);
        result.emplace_back(std::to_string(static_cast<int>(*value)));
        return result;
    };
}

void InspectorParam::GenerateResourceFunc() {
    m_paramFunc = [this]() {
        auto* value = static_cast<CSE::SResource*>(m_value);
        const auto& name = ((value == nullptr) ? std::string("None") : (value->GetName())
                           + " (" + value->GetClassType() + ")");
        ImGui::Button(name.c_str());
        
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("INSP_RES", value, sizeof(CSE::SResource));
            ImGui::EndDragDropSource();
        }
        
        if (ImGui::BeginDragDropTarget()) {
            if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_RES")) {
                const auto& dropObject = static_cast<CSE::SResource*>(payload->Data);
                if (m_classType != dropObject->GetClassType()) {
                    ImGui::EndDragDropTarget();
                    return false;
                }
                const auto& str = dropObject->GetHash();
                ReplaceValueString(str.c_str(), str.size());
                ImGui::EndDragDropTarget();
                return true;
            }
            if (const auto& payload = ImGui::AcceptDragDropPayload("AW_RES")) {
                const auto& dropObject = static_cast<CSE::AssetMgr::AssetReference*>(payload->Data);
                if (m_classType != dropObject->class_type) {
                    ImGui::EndDragDropTarget();
                    return false;
                }
                const auto& res = CSE::SResource::Create(dropObject, dropObject->class_type);
                if(res == m_value) {
                    ImGui::EndDragDropTarget();
                    return false;
                }
                m_value = res;
                ImGui::EndDragDropTarget();
                return true;
            }
            ImGui::EndDragDropTarget();
        }
        return false;
    };
    m_getFunc = [this]() {
        std::vector<std::string> result;
        const auto& value = static_cast<CSE::SResource*>(m_value);
        result.reserve(1);
        result.emplace_back(value->GetHash());
        return result;
    };
}

void InspectorParam::GenerateObjectFunc() {
    m_paramFunc = [this]() {
        auto* value = static_cast<CSE::SGameObject*>(m_value);
        const auto& name =  ((value == nullptr) ? std::string("null") : value->GetName())
                            + " (GameObject)";
        ImGui::Button(name.c_str());
        
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("INSP_GOBJ", value, sizeof(CSE::SGameObject));
            ImGui::EndDragDropSource();
        }
        
        if (ImGui::BeginDragDropTarget()) {
            if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_GOBJ")) {
                const auto& dropObject = static_cast<CSE::SGameObject*>(payload->Data);
                m_value = dropObject;
                ImGui::EndDragDropTarget();
                return true;
            }
            ImGui::EndDragDropTarget();
        }
        return false;
    };
    m_getFunc = [this]() {
        std::vector<std::string> result;
        const auto& value = static_cast<CSE::SGameObject*>(m_value);
        result.reserve(1);
        result.emplace_back(value->GetHash());
        return result;
    };
}

void InspectorParam::GenerateComponentFunc() {
    m_paramFunc = [this]() {
        auto* value = static_cast<CSE::SComponent*>(m_value);
        const auto& name =  ((value == nullptr) ? std::string("null") : value->GetGameObject()->GetName())
                            + " (" + value->GetClassType() + ")";
        ImGui::Button(name.c_str());
        
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("INSP_COMP", value, sizeof(CSE::SComponent));
            ImGui::EndDragDropSource();
        }
        
        if (ImGui::BeginDragDropTarget()) {
            if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_COMP")) {
                const auto& dropComponent = static_cast<CSE::SComponent*>(payload->Data);
                if (m_classType != dropComponent->GetClassType()) {
                    ImGui::EndDragDropTarget();
                    return false;
                }
                m_value = dropComponent;
                ImGui::EndDragDropTarget();
                return true;
            }
            if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_GOBJ")) {
                const auto& dropObject = static_cast<CSE::SGameObject*>(payload->Data);
                const auto& dropComponents = dropObject->GetComponents();
                CSE::SComponent* target = nullptr;
                for (const auto& component: dropComponents) {
                    if (m_classType != component->GetClassType()) continue;
                    target = component;
                    break;
                }
                m_value = target;
                ImGui::EndDragDropTarget();
                return target != nullptr;
            }
            ImGui::EndDragDropTarget();
        }
        return false;
    };
    m_getFunc = [this]() {
        std::vector<std::string> result;
        const auto& value = static_cast<CSE::SComponent*>(m_value);
        result.reserve(1);
        result.emplace_back(value->GetGameObject()->GetHash() + '?' + value->GetClassType());
        return result;
    };
}
