#pragma once
#include "../MacroDef.h"
#include "MemoryContainer.h"
#include "Base/CoreBase.h"

namespace CSE {

    class MemoryMgr : public MemoryContainer, public CoreBase {
    public:
        explicit MemoryMgr();
        ~MemoryMgr() override;

    public:
        void ExterminateObjects(bool killAll = false);

        void ReleaseObject(SObject* object, bool isForce = false);

        void Init() override;

    private:

    };
}