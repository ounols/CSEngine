//
// Created by ounols on 19. 11. 11.
//

#include "MainDocker.h"
#include "imgui_internal.h"
#include "../../src/MacroDef.h"
#include "../Manager/EEngineCore.h"

#include "InspectorWindow.h"
#include "PreviewWindow.h"
#include "HierarchyWindow.h"
#include "ConsoleWindow.h"
#include "AssetWindow.h"

using namespace CSEditor;

namespace CSEMainDocker {
    PreviewWindow* previewWindow = nullptr;
    HierarchyWindow* hierarchyWindow = nullptr;
    InspectorWindow* inspectorWindow = nullptr;
}

MainDocker::MainDocker() {

}

MainDocker::~MainDocker() {
    for (auto window: m_windows) {
        SAFE_DELETE(window);
    }
    m_windows.clear();
    EEngineCore::getEditorInstance()->Exterminate();
    EEngineCore::delInstance();
}

void MainDocker::SetUI() {
    if (!m_bIsInit) GenerateWindows();

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
    if (!m_bIsInit) SetDockerNodes();
    ImGui::End();

    SetWindowsUI();
}

void MainDocker::SetMenuBar() const {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "CTRL+Z");
            ImGui::MenuItem("Redo", "CTRL+Y", false, false); // Disabled item
            ImGui::Separator();
            ImGui::MenuItem("Cut", "CTRL+X");
            ImGui::MenuItem("Copy", "CTRL+C");
            ImGui::MenuItem("Paste", "CTRL+V");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            if (CSEMainDocker::previewWindow->IsPreview()) {
                if (ImGui::MenuItem("Stop Preview")) {
                    CSEMainDocker::hierarchyWindow->ClearSelectedObject();
                    CSEMainDocker::previewWindow->ReleasePreview();
                }
            } else {
                if (ImGui::MenuItem("Start Preview")) {
                    CSEMainDocker::hierarchyWindow->ClearSelectedObject();
                    CSEMainDocker::previewWindow->InitPreview();
                }
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
    auto dockDown = ImGui::DockBuilderSplitNode(m_dockerspaceId, ImGuiDir_Down, 0.4f, nullptr, &m_dockerspaceId);
    auto dockLeft = ImGui::DockBuilderSplitNode(m_dockerspaceId, ImGuiDir_Left, 0.25f, nullptr, &m_dockerspaceId);
    ImGui::DockBuilderDockWindow("Preview", m_dockerspaceId);
    ImGui::DockBuilderDockWindow("Inspector", dockRight);
    ImGui::DockBuilderDockWindow("Console Log", dockDown);
    ImGui::DockBuilderDockWindow("Assets Explorer", dockDown);
    ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
    ImGui::DockBuilderFinish(m_dockerspaceId);
    m_bIsInit = true;
}

void MainDocker::GenerateWindows() {
    CSEMainDocker::previewWindow = new PreviewWindow();
    CSEMainDocker::hierarchyWindow = new HierarchyWindow();
    CSEMainDocker::inspectorWindow = new InspectorWindow();

    m_windows.reserve(4);
    m_windows.push_back(CSEMainDocker::inspectorWindow);
    m_windows.push_back(CSEMainDocker::previewWindow);
    m_windows.push_back(CSEMainDocker::hierarchyWindow);
    m_windows.push_back(new ConsoleWindow());
    m_windows.push_back(new AssetWindow());

    for (const auto& window: m_windows) {
        window->Register(this);
    }
}

void MainDocker::SetWindowsUI() {
    for (const auto& window: m_windows) {
        window->SetUI();
    }
}

void MainDocker::Reset() {
    CSEMainDocker::hierarchyWindow->ClearSelectedObject();
    CSEMainDocker::inspectorWindow->ReleaseLayers();
}
