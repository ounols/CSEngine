#pragma once

#include <string>
#include <utility>
#include "../../Manager/EngineCore.h"
#include "../../Manager/ReflectionMgr.h"

namespace CSE {
    class ReflectionObject {
    public:
        ReflectionObject() = default;
        explicit ReflectionObject(std::string type) : m_class(std::move(type)) {}

        virtual ~ReflectionObject() = default;

        void SetClassType(std::string type) {
            m_class = std::move(type);
        }

        const char* GetClassType() const {
            return m_class.c_str();
        }

        static ReflectionObject* NewObject(const std::string& name) {
            return CORE->GetReflectionMgrCore()->CreateObject(name);
        }

    protected:
        std::string m_class;
    };
}