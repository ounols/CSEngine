//
// Created by ounols on 19. 10. 12.
//

#pragma once

#include "imgui.h"

namespace CSEditor {

    class MainDocker;

    class WindowBase {
    public:
        WindowBase() {
            m_mainViewport = ImGui::GetMainViewport();
        }

        void Register(MainDocker* mainDocker) {
            m_mainDocker = mainDocker;
        }

        virtual ~WindowBase() = default;

        virtual void SetUI() = 0;


    protected:
        bool m_isClosed = true;
        ImGuiViewport* m_mainViewport = nullptr;
        MainDocker* m_mainDocker = nullptr;
    };


}