//
// Created by ounols on 19. 11. 11.
//

#pragma once
#define __CSE_EDITOR__

#include "Base/WindowBase.h"
#include <vector>

namespace CSEditor {

    class MainDocker : public WindowBase {
    public:
        MainDocker();

        ~MainDocker();

        void SetUI() override;

    private:
        void SetMenuBar() const;

        void SetDockerNodes();

        void GenerateWindows();

        void SetWindowsUI();

    private:
        bool m_bIsInit = false;
        unsigned int m_dockerspaceId;
        ImGuiWindowFlags m_windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        std::vector<WindowBase*> m_windows;
    };

}