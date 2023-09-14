#pragma once

#include <string>
#include <utility>

namespace CSE {
    class ReflectionObject {
    public:
        ReflectionObject() = default;
        explicit ReflectionObject(std::string type) : m_class(std::move(type)) {

        }

        virtual ~ReflectionObject() = default;

        void SetClassType(std::string type) {
            m_class = std::move(type);
        }

        const char* GetClassType() const {
            return m_class.c_str();
        }

        virtual ReflectionObject* NewObject(const std::string& name) = 0;

    protected:
        std::string m_class;
    };
}