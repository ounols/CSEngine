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
        
        void GenerateIntFunc(const char* name);
        void GenerateFloatFunc(const char* name);
        void GenerateVec2Func(const char* name);
        void GenerateVec3Func(const char* name);
        void GenerateColor3Func(const char* name);
        void GenerateVec4Func(const char* name);
        void GenerateColor4Func(const char* name);
        void GenerateStringFunc(const char* name);
        void GenerateBoolFunc(const char* name);
        void GenerateResourceFunc();
        void GenerateObjectFunc();
        void GenerateComponentFunc();

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