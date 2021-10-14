#pragma once
#include "../SObject.h"
#include "Base/SContainer.h"

namespace CSE {

    class SObject;

    class MemoryContainer : public SContainer<SObject*> {

    protected:
        MemoryContainer();

        ~MemoryContainer() override;

    };

}