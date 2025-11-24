//
// Created by ounols on 19. 11. 11.
//

#include "MainDocker.h"
#include "imgui_internal.h"
#include "../../src/MacroDef.h"
#include "../Manager/EEngineCore.h"
#include "../Manager/EditorActionLogger.h"
#include "../../src/Util/Loader/SCENE/SSceneLoader.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Component/LightComponent.h"
#include "../../src/Component/CameraComponent.h"

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
    AssetWindow* assetWindow = nullptr;
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
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                ACTION_LOG_EDITOR("Menu: File > New Scene");
                // TODO: Create new scene
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                ACTION_LOG_EDITOR("Menu: File > Open Scene");
                // TODO: Open scene file dialog
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                ACTION_LOG_EDITOR("Menu: File > Save Scene");
                CSEMainDocker::assetWindow->SaveCurrentScene();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                ACTION_LOG_EDITOR("Menu: File > Exit");
                // TODO: Proper exit handling
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Y", false, false); // Disabled item
            ImGui::Separator();
            ImGui::MenuItem("Cut", "Ctrl+X");
            ImGui::MenuItem("Copy", "Ctrl+C");
            ImGui::MenuItem("Paste", "Ctrl+V");
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
                ACTION_LOG_EDITOR("Menu: Edit > Duplicate");
                CSEMainDocker::hierarchyWindow->DuplicateSelectedGameObject();
            }
            if (ImGui::MenuItem("Delete", "Delete")) {
                ACTION_LOG_EDITOR("Menu: Edit > Delete");
                CSEMainDocker::hierarchyWindow->DeleteSelectedGameObject();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N")) {
                CSEMainDocker::hierarchyWindow->CreateEmptyGameObject();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("3D Object")) {
                if (ImGui::MenuItem("Cube")) {
                    CSEMainDocker::hierarchyWindow->CreatePrimitiveGameObject("Cube");
                }
                if (ImGui::MenuItem("Sphere")) {
                    CSEMainDocker::hierarchyWindow->CreatePrimitiveGameObject("Sphere");
                }
                if (ImGui::MenuItem("Plane")) {
                    CSEMainDocker::hierarchyWindow->CreatePrimitiveGameObject("Plane");
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Light")) {
                if (ImGui::MenuItem("Directional Light")) {
                    auto* obj = CSEMainDocker::hierarchyWindow->CreateEmptyGameObject();
                    if (obj) {
                        obj->SetName("Directional Light");
                        auto* light = obj->CreateComponent<CSE::LightComponent>();
                        light->SetLightType(CSE::LightComponent::DIRECTIONAL);
                    }
                }
                if (ImGui::MenuItem("Point Light")) {
                    auto* obj = CSEMainDocker::hierarchyWindow->CreateEmptyGameObject();
                    if (obj) {
                        obj->SetName("Point Light");
                        auto* light = obj->CreateComponent<CSE::LightComponent>();
                        light->SetLightType(CSE::LightComponent::POINT);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Camera")) {
                auto* obj = CSEMainDocker::hierarchyWindow->CreateEmptyGameObject();
                if (obj) {
                    obj->SetName("Camera");
                    obj->CreateComponent<CSE::CameraComponent>();
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            bool isPreview = CSEMainDocker::previewWindow->IsPreview();

            // Play/Stop button
            if (isPreview) {
                if (ImGui::MenuItem("Stop", "Ctrl+P")) {
                    ACTION_LOG_EDITOR("Menu: Debug > Stop", "Preview mode stopped");
                    CSEMainDocker::hierarchyWindow->ClearSelectedObject();
                    CSEMainDocker::previewWindow->ReleasePreview();
                }
            } else {
                if (ImGui::MenuItem("Play", "Ctrl+P")) {
                    ACTION_LOG_EDITOR("Menu: Debug > Play", "Preview mode started");
                    CSEMainDocker::hierarchyWindow->ClearSelectedObject();
                    CSEMainDocker::previewWindow->InitPreview();
                }
            }

            ImGui::Separator();

            // Debug options
            ImGui::MenuItem("Show FPS", nullptr, false, false);
            ImGui::MenuItem("Show Stats", nullptr, false, false);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Hierarchy", nullptr, true);
            ImGui::MenuItem("Inspector", nullptr, true);
            ImGui::MenuItem("Preview", nullptr, true);
            ImGui::MenuItem("Assets", nullptr, true);
            ImGui::MenuItem("Console", nullptr, true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About CSEngine Editor")) {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Global keyboard shortcuts
    HandleGlobalShortcuts();
}

void MainDocker::HandleGlobalShortcuts() const {
    // Ctrl+S - Save scene
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
        ACTION_LOG_EDITOR("Shortcut: Ctrl+S", "Save Scene");
        CSEMainDocker::assetWindow->SaveCurrentScene();
    }

    // Ctrl+P - Play/Stop preview
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_P)) {
        if (CSEMainDocker::previewWindow->IsPreview()) {
            ACTION_LOG_EDITOR("Shortcut: Ctrl+P", "Stop Preview");
            CSEMainDocker::hierarchyWindow->ClearSelectedObject();
            CSEMainDocker::previewWindow->ReleasePreview();
        } else {
            ACTION_LOG_EDITOR("Shortcut: Ctrl+P", "Start Preview");
            CSEMainDocker::hierarchyWindow->ClearSelectedObject();
            CSEMainDocker::previewWindow->InitPreview();
        }
    }

    // Ctrl+D - Duplicate
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D)) {
        ACTION_LOG_EDITOR("Shortcut: Ctrl+D", "Duplicate");
        CSEMainDocker::hierarchyWindow->DuplicateSelectedGameObject();
    }

    // Delete - Delete selected (only when not typing)
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !ImGui::GetIO().WantTextInput) {
        ACTION_LOG_EDITOR("Shortcut: Delete", "Delete Selected");
        CSEMainDocker::hierarchyWindow->DeleteSelectedGameObject();
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
    CSEMainDocker::assetWindow = new AssetWindow();

    m_windows.reserve(4);
    m_windows.push_back(CSEMainDocker::inspectorWindow);
    m_windows.push_back(CSEMainDocker::previewWindow);
    m_windows.push_back(CSEMainDocker::hierarchyWindow);
    m_windows.push_back(CSEMainDocker::assetWindow);
    m_windows.push_back(new ConsoleWindow());

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
