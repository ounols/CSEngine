#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {
    class PreviewWindow : public WindowBase {
    public:
        PreviewWindow();

        ~PreviewWindow() override;

        void SetUI() override;

    };
}