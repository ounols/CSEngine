#include "PreviewWindow.h"
#include "../Manager/EEngineCore.h"

using namespace CSEditor;

PreviewWindow::PreviewWindow() {
    m_engineCore = EEngineCore::getEditorInstance();
    m_engineCore->InvokeEditorRender();
}

PreviewWindow::~PreviewWindow() {
    m_engineCore = nullptr;
}

void PreviewWindow::SetUI() {
    ImGui::Begin("Preview");
    const unsigned int width = ImGui::GetContentRegionAvail().x;
    const unsigned int height = ImGui::GetContentRegionAvail().y;

    if (m_bIsInit) {
        m_engineCore->InvokePreviewStart(width, height);
        m_bIsInit = false;
        m_prevWidth = width;
        m_prevHeight = height;
    }

    if ((m_prevWidth != width || m_prevHeight != height)) {
        m_engineCore->InvokePreviewResize(width, height);
        m_engineCore->InvokeEditorRender();
    }

    if (!m_engineCore->IsPreview() && ImGui::IsWindowHovered()) {
        for (ImGuiKey key = static_cast<ImGuiKey>(0); key < ImGuiKey_COUNT; key = (ImGuiKey) (key + 1)) {
            if (ImGui::IsKeyDown(key)) {
                m_engineCore->InvokeEditorRender();
                break;
            }
        }
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddImage(
            (void*) m_engineCore->GetPreviewTextureId(),
            ImVec2(pos.x, pos.y),
            ImVec2(pos.x + width, pos.y + height),
            ImVec2(0, 1),
            ImVec2(1, 0)
    );

    m_prevWidth = width;
    m_prevHeight = height;
    ImGui::End();
}

void PreviewWindow::InitPreview() {
    m_bIsInit = true;
}

void PreviewWindow::ReleasePreview() {
    m_engineCore->InvokePreviewStop();
    m_bIsInit = false;
}

bool PreviewWindow::IsPreview() const {
    return m_engineCore->IsPreview();
}
