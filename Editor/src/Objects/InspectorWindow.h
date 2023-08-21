#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {
    class InspectorWindow : public WindowBase {
    public:
        InspectorWindow();

        ~InspectorWindow() override;

        void SetUI() override;
    };
}