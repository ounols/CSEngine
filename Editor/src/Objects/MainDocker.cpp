//
// Created by ounols on 19. 11. 11.
//

#include "MainDocker.h"
#include "imgui_internal.h"
#include "../../src/MacroDef.h"

#include "InspectorWindow.h"
#include "PreviewWindow.h"

using namespace CSEditor;

MainDocker::MainDocker() {

}

MainDocker::~MainDocker() {
    for (auto window : m_windows) {
        SAFE_DELETE(window);
    }
    m_windows.clear();
}

void MainDocker::SetUI() {
    if(!m_bIsInit) GenerateWindows();

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
    m_dockerspaceId = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(m_dockerspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    SetMenuBar();
    if(!m_bIsInit) SetDockerNodes();
    ImGui::End();

    SetWindowsUI();
}

void MainDocker::SetMenuBar() const {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Start Game")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void MainDocker::SetDockerNodes() {
    ImGui::DockBuilderRemoveNode(m_dockerspaceId);
    ImGui::DockBuilderAddNode(m_dockerspaceId, m_windowFlags | ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(m_dockerspaceId, m_mainViewport->WorkSize);

    auto dockRight = ImGui::DockBuilderSplitNode(m_dockerspaceId, ImGuiDir_Right, 0.25f, nullptr, &m_dockerspaceId);
    ImGui::DockBuilderDockWindow("Preview", m_dockerspaceId);
    ImGui::DockBuilderDockWindow("Inspector", dockRight);
    ImGui::DockBuilderFinish(m_dockerspaceId);
    m_bIsInit = true;
}

void MainDocker::GenerateWindows() {
    m_windows.reserve(2);
    m_windows.push_back(new InspectorWindow());
    m_windows.push_back(new PreviewWindow());
}

void MainDocker::SetWindowsUI() {
    for (const auto& window : m_windows) {
        window->SetUI();
    }
}
