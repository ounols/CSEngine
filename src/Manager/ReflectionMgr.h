#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include "Base/CoreBase.h"

namespace CSE {

    class ReflectionObject;

    class ReflectionMgr : public CoreBase {
    public:
        class DefineWrapper {
        private:
            struct DefineWrapperNode {
                std::string m_name;
                std::function<ReflectionObject*()> m_func;
                DefineWrapperNode* m_next = nullptr;

                DefineWrapperNode(std::string name, std::function<ReflectionObject*()> func) : m_name(std::move(name)),
	                m_func(std::move(func)) {}
            };

            typedef DefineWrapperNode* WrapperContainer;

        public:
            DefineWrapper() = default;

            DefineWrapper(const DefineWrapper& other) : m_defined(other.m_defined), m_node(other.m_node) {}

            static unsigned char* SetDefine(std::string&& type, std::function<ReflectionObject*()>&& func) {
                if (ReflectionMgr::m_defineWrapper.m_defined == nullptr) {
                    ReflectionMgr::m_defineWrapper.m_defined = new DefineWrapperNode(type, func);
                    ReflectionMgr::m_defineWrapper.m_node = ReflectionMgr::m_defineWrapper.m_defined;
                    return nullptr;
                }
                auto& src = ReflectionMgr::m_defineWrapper.m_node;
                auto* new_obj = new DefineWrapperNode(type, func);
                src->m_next = new_obj;
                ReflectionMgr::m_defineWrapper.m_node = new_obj;
                ReflectionMgr::m_defineWrapper_prev = m_defineWrapper;
                return nullptr;
            }

            static void ReleaseDefine() {
                for (auto* node = ReflectionMgr::m_defineWrapper.m_defined;;) {
                    if (node == nullptr) break;
                    auto* node_next = node->m_next;
                    delete node;
                    node = node_next;
                }
            }

        private:
            WrapperContainer m_defined = nullptr;
            WrapperContainer m_node = nullptr;

        public:
            friend ReflectionMgr;
        };

    public:
        explicit ReflectionMgr();

        ~ReflectionMgr() override;

        void Init() override;
        ReflectionObject* CreateObject(const std::string& type);

    private:
        static DefineWrapper m_defineWrapper;
        static DefineWrapper m_defineWrapper_prev;

        std::unordered_map<std::string, std::function<ReflectionObject*()>> m_reflected;
    };

#ifdef __CSE_REFLECTION_ENABLE__
    CSE::ReflectionMgr::DefineWrapper CSE::ReflectionMgr::m_defineWrapper = CSE::ReflectionMgr::DefineWrapper(CSE::ReflectionMgr::m_defineWrapper_prev);
    CSE::ReflectionMgr::DefineWrapper CSE::ReflectionMgr::m_defineWrapper_prev = CSE::ReflectionMgr::DefineWrapper();
#endif
}
