#pragma once

#include "../../../src/Manager/EngineCoreInstance.h"

namespace CSEditor {

    class EEngineCore;

    class EPreviewCore : public CSE::EngineCoreInstance {
    protected:
        EPreviewCore();
        ~EPreviewCore() override;

    public:
        friend EEngineCore;
    };

}
