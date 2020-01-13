//
// Created by ounols on 19. 11. 11.
//

#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {

    class MainDocker : public WindowBase {
    public:
        MainDocker();
        ~MainDocker();

        void SetUI() override;
    };

}