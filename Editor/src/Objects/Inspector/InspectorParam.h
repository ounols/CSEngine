#pragma once

#include <functional>
#include <string>
#include <vector>
#include "InspectorParamType.h"

class XNode;

namespace CSEditor {
    class InspectorParam {
    public:
        explicit InspectorParam(const XNode& node);
        ~InspectorParam();

        bool PrintUI();
        std::vector<std::string> GetParam();
        void UpdateParam(const XNode& node);
        std::string& GetName() {
            return m_name;
        }

    private:
        void GenerateValue(const XNode& node);
        void GenerateFunc();
        void ReplaceValueString(const char* str, const int size);

    private:
        std::string m_name;
        std::string m_nameId;
        InspectorParamType m_type = InspectorParamType::UNKNOWN;
        std::function<bool()> m_paramFunc = nullptr;
        std::function<void()> m_deleteFunc = nullptr;
        std::function<void(const XNode&)> m_updateFunc = nullptr;
        std::function<std::vector<std::string>()> m_getFunc = nullptr;
        void* m_value = nullptr;
        std::string m_classType = "";
    };
}