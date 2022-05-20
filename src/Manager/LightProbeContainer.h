#pragma once
#include "Base/SContainer.h"

namespace CSE {

    class LightProbeGroupComponent;

    class LightProbeContainer : public SContainer<LightProbeGroupComponent*> {
    public:
        LightProbeContainer();
        ~LightProbeContainer() override;
    };

}
