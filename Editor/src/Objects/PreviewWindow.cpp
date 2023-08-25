#include "PreviewWindow.h"
#include "../Manager/EEngineCore.h"

using namespace CSEditor;

PreviewWindow::PreviewWindow() {
    m_engineCore = EEngineCore::getEditorInstance();
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

    if (m_engineCore->IsPreview()
        && (m_prevWidth != width || m_prevHeight != height)) {
        m_engineCore->InvokePreviewResize(width, height);
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
