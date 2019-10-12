#pragma once
#include "../MacroDef.h"
#include "MemoryContainer.h"

namespace CSE {

    class MemoryMgr : public MemoryContainer {
    protected:
        ~MemoryMgr();

    DECLARE_SINGLETONE(MemoryMgr);

    public:
        void ExterminateObjects(bool killAll = false);

        void ReleaseObject(SObject* object, bool isForce = false);

    private:

    };
}