#include "ReflectionMgr.h"

using namespace CSE;

ReflectionMgr::ReflectionMgr() {

}

ReflectionMgr::~ReflectionMgr() {

}

void ReflectionMgr::Init() {

}

void ReflectionMgr::Register(ReflectionObject* object) {

}

void ReflectionMgr::Remove(ReflectionObject* object) {
    std::string typeName = object->GetClassType();
    if (m_objects.count(typeName) <= 0) return;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second == object) {
            m_objects.erase(it);
            break;
        }
    }
}

ReflectionObject* ReflectionMgr::Get(std::string index) const {
    return m_objects.at(index);
}

std::unordered_map<std::string, ReflectionObject*> ReflectionMgr::GetAll() const {
    return m_objects;
}

std::string ReflectionMgr::GetID(ReflectionObject* object) const {
    return object->GetClassType();
}

int ReflectionMgr::GetSize() const {
    return m_objects.size();
}
