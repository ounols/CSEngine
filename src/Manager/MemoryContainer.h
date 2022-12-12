#pragma once

#include "../SObject.h"
#include "Base/SContainerList.h"
#include <set>

namespace CSE {

    class SObject;

    class MemoryContainer : public SContainerList<SObject*> {
    protected:
        MemoryContainer();

        ~MemoryContainer() override;

    public:
        void Register(SObject* object) override;

        void Remove(SObject* object) override;

        bool HasHash(const std::string& hash) const;

        void ChangeHash(const std::string& srcHash, const std::string& dstHash);

    protected:
        std::set<std::string> m_hashContainer;
    };
}