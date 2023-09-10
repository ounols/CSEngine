#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {
    class AssetWindow : public WindowBase {
    public:
        AssetWindow();
        ~AssetWindow() override;

        void SetUI() override;
    };
}