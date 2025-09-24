#pragma once

#include <string>
#include <utility>
#include "../../Manager/EngineCore.h"
#include "../../Manager/ReflectionMgr.h"

namespace CSE {
    class ReflectionObject {
    public:
        ReflectionObject() = default;

        explicit ReflectionObject(const std::string& type) : m_class(type) {}

        virtual ~ReflectionObject() = default;

        void SetClassType(const std::string& type) {
            m_class = type;
        }

        const char* GetClassType() const {
            return m_class.c_str();
        }

        bool IsSameClass(const char* classType) const {
            return std::equal(m_class.begin(), m_class.end(), classType);
        }

        static ReflectionObject* NewObject(const std::string& name) {
            return CORE->GetReflectionMgrCore()->CreateObject(name);
        }

    protected:
        std::string m_class;
    };
}