//
// Created by ounols on 19. 11. 11.
//

#include "MainDocker.h"

using namespace CSEditor;

MainDocker::MainDocker() {

}

MainDocker::~MainDocker() {

}

void MainDocker::SetUI() {
    ImGui::SetNextWindowPos(m_mainViewport->WorkPos);
    ImGui::SetNextWindowSize(m_mainViewport->WorkSize);
    ImGui::SetNextWindowViewport(m_mainViewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove;
    m_windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MainDockSpace", nullptr, m_windowFlags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    auto dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}
