#include "ReflectionMgr.h"
#include "../Object/Base/ReflectionObject.h"

using namespace CSE;

ReflectionMgr::ReflectionMgr() {

}

ReflectionMgr::~ReflectionMgr() {

}

void ReflectionMgr::Init() {
    for (auto* node = ReflectionMgr::m_defineWrapper.m_defined;;) {
        if (node == nullptr) break;
        auto* node_next = node->m_next;
        m_reflected[node->m_name] = node->m_func;
        node = node_next;
    }
}

ReflectionObject* ReflectionMgr::CreateObject(const std::string& type) {
    const auto& func = m_reflected.find(type);
    if(func == m_reflected.end()) return nullptr;
    return func->second();
}
