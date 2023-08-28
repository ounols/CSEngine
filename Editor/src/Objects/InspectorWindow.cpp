#include "InspectorWindow.h"
#include "Base/HierarchyData.h"
#include "../Manager/EEngineCore.h"
#include "../../src/Object/SGameObject.h"
#include "../../src/Util/Loader/XML/XML.h"
#include <regex>

using namespace CSEditor;
using namespace CSE;

InspectorWindow::InspectorWindow() {
    m_core = EEngineCore::getEditorInstance();
}

InspectorWindow::~InspectorWindow() {

}

void InspectorWindow::SetUI() {
    ImGui::Begin("Inspector");

    const auto& selected = m_core->GetHierarchyData()->GetSelectedObject();

    if (selected == nullptr) {
        ImGui::End();
        return;
    }

    const auto& components = selected->GetComponents();

    for (const auto& component: components) {
        if (ImGui::CollapsingHeader(component->GetClassType().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            const auto buffer = component->PrintValue();
            if (buffer.empty()) {
                ImGui::Text("No Data");
                continue;
            }
            const XNode* root = XFILE().loadBuffer(buffer);
            const auto& componentNode = root->getChild("component");
            ImGui::BeginTable(component->GetHash().c_str(), 2, ImGuiTableFlags_None);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Enable");
            ImGui::TableNextColumn();
            bool enable = std::stoi(componentNode.getAttribute("enable").value);
            ImGui::Checkbox("", &enable);

            for (const auto& valueNode: componentNode.children) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", valueNode.getAttribute("name").value.c_str());
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-FLT_MIN);
                GenerateValueInputs(valueNode);
                ImGui::PopItemWidth();

            }
            ImGui::EndTable();
            delete root;
        }
    }

    ImGui::End();
}

void InspectorWindow::GenerateValueInputs(const XNode& buffer) {
    static const std::regex intRegex("^[-+]?[0-9]+$");
    static const std::regex floatRegex(R"("^[-+]?[1-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$")");

    const auto& values = buffer.value.toStringVector();
    const auto& size = values.size();
    if(size <= 0) return;
    bool isFloat = std::regex_match(trim(values[0]), intRegex) || std::regex_match(trim(values[0]), floatRegex);

    std::vector<int> intValues;
    std::vector<float> floatValues;

//    if(isInt) {
//        intValues.reserve(size);
//        for(const auto& v : values) {
//            intValues.emplace_back(std::stoi(v));
//        }
//    }
    if (isFloat) {
        floatValues.reserve(size);
        for(const auto& v : values) {
            floatValues.emplace_back(std::stof(v));
        }
    }

    if (size == 4) {
        if(isFloat) ImGui::InputFloat4("", &floatValues[0]);
    }
    else if (size == 3) {
        if(isFloat) ImGui::InputFloat3("", &floatValues[0]);
    }
    else if (size == 2) {
        if(isFloat) ImGui::InputFloat2("", &floatValues[0]);
    }
    else if (size == 1) {
        if(isFloat) ImGui::InputFloat("", &floatValues[0]);
        else        ImGui::InputText("", const_cast<char*>(values[0].c_str()), 24);
    }
}
