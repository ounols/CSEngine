#pragma once

#include "ReflectionObject.h"

namespace CSE {
    template<class T>
    class ReflectionRef {
    public:
        explicit ReflectionRef(std::string type) : m_class(std::move(type)) {}
        explicit ReflectionRef() {
            const char* type = T::GetClassStaticType();
            m_class = type;
        }

        ~ReflectionRef() = default;

        bool IsSameClass(const ReflectionObject* object) const {
            if(object == nullptr) return false;
            return std::strcmp(m_class.c_str(), object->GetClassType()) == 0;
        }

    private:
        std::string m_class;
    };
}