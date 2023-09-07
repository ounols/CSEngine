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
        case InspectorParamType::RES:
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
            m_updateFunc = [ptr](const XNode& node) {
                memset(ptr, 0, sizeof(char) * 128);
                const auto& str = node.value.c_str();
                const int size = node.value.size();
                for (int i = 0; i < size; ++i) {
                    ptr[i] = str[i];
                }
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
        case InspectorParamType::INT: {
            auto* value = static_cast<int*>(m_value);
            m_paramFunc = [value, name]() {
                return ImGui::InputInt(name, value);
            };
            m_getFunc = [value]() {
                std::vector<std::string> result;
                result.reserve(1);
                result.emplace_back(std::to_string(*value));
                return result;
            };
            break;
        }
        case InspectorParamType::FLOAT: {
            auto* value = static_cast<float*>(m_value);
            m_paramFunc = [value, name]() {
                return ImGui::InputFloat(name, value);
            };
            m_getFunc = [value]() {
                std::vector<std::string> result;
                result.reserve(1);
                result.emplace_back(std::to_string(*value));
                return result;
            };
            break;
        }
        case InspectorParamType::VEC2: {
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
            break;
        }
        case InspectorParamType::VEC3: {
            auto* value = static_cast<CSE::vec3*>(m_value);
            m_paramFunc = [value, name]() {
                return ImGui::InputFloat3(name, const_cast<float*>(value->Pointer()));
            };
            m_getFunc = [value]() {
                std::vector<std::string> result;
                result.reserve(3);
                result.emplace_back(std::to_string(value->x));
                result.emplace_back(std::to_string(value->y));
                result.emplace_back(std::to_string(value->z));
                return result;
            };
            break;
        }
        case InspectorParamType::COLOR3: {
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
            break;
        }
        case InspectorParamType::VEC4: {
            auto* value = static_cast<CSE::vec4*>(m_value);
            m_paramFunc = [value, name]() {
                return ImGui::InputFloat4(name, const_cast<float*>(value->Pointer()));
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
            break;
        }
        case InspectorParamType::COLOR4: {
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
            break;
        }
        case InspectorParamType::STRING: {
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
            break;
        }
        case InspectorParamType::BOOL: {
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
            break;
        }
        case InspectorParamType::RES: {
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
            break;
        }
        case InspectorParamType::OBJ: {
            m_paramFunc = [this]() {
                auto* value = static_cast<CSE::SGameObject*>(m_value);
                const auto& name = value->GetName() + " (GameObject)";
                ImGui::Button(name.c_str());
                // When Dragging
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("INSP_GOBJ", value, sizeof(CSE::SGameObject));
                    ImGui::EndDragDropSource();
                }
                // When Dropped
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
            break;
        }
        case InspectorParamType::COMP: {
            m_paramFunc = [this]() {
                auto* value = static_cast<CSE::SComponent*>(m_value);
                const auto& name = value->GetGameObject()->GetName() + " (" + value->GetClassType() + ")";
                ImGui::Button(name.c_str());
                // When Dragging
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("INSP_COMP", value, sizeof(CSE::SComponent));
                    ImGui::EndDragDropSource();
                }
                // When Dropped
                if (ImGui::BeginDragDropTarget()) {
                    if (const auto& payload = ImGui::AcceptDragDropPayload("INSP_COMP")) {
                        const auto& dropComponent = static_cast<CSE::SComponent*>(payload->Data);
                        if (value->GetClassType() != dropComponent->GetClassType()) {
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
                            if (value->GetClassType() != component->GetClassType()) continue;
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
            break;
        }
        default:
            break;
    }
}