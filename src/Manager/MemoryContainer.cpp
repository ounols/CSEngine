#include "MemoryContainer.h"

using namespace CSE;

MemoryContainer::MemoryContainer() = default;


MemoryContainer::~MemoryContainer() = default;

void MemoryContainer::Register(SObject* object) {
    const auto& prevSize = m_hashContainer.size();
    m_hashContainer.insert(object->GetHash());
    if(prevSize - m_hashContainer.size() == 0) throw -1;
    SContainerList::Register(object);
}

void MemoryContainer::Remove(SObject* object) {
    m_hashContainer.erase(object->GetHash());
    SContainerList::Remove(object);
}

bool MemoryContainer::HasHash(const std::string& hash) const {
    return m_hashContainer.count(hash) > 0;
}

void MemoryContainer::ChangeHash(const std::string& srcHash, const std::string& dstHash) {
    m_hashContainer.erase(srcHash);
    const auto& prevSize = m_hashContainer.size();
    m_hashContainer.insert(dstHash);
    //if(prevSize - m_hashContainer.size() == 0) throw -1;
}