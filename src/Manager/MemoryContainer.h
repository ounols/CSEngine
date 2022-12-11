#pragma once
#include "../SObject.h"
#include "Base/SContainerList.h"

namespace CSE {

    class SObject;

    class MemoryContainer : public SContainerList<SObject*> {

    protected:
        MemoryContainer();

        ~MemoryContainer() override;

    };

}