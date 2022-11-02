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

    private:
        ImGuiWindowFlags m_windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    };

}