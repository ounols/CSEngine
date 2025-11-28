#include "PreviewWindow.h"
#include "../Manager/EEngineCore.h"
#include "../Backend/EditorBackend.h"

using namespace CSEditor;

PreviewWindow::PreviewWindow() {
    m_engineCore = EEngineCore::getEditorInstance();
    EditorBackend::GetInstance().InvokeEditorRender();
}

PreviewWindow::~PreviewWindow() {
    m_engineCore = nullptr;
}

void PreviewWindow::SetUI() {
    auto& backend = EditorBackend::GetInstance();

    ImGui::Begin("Preview");
    const unsigned int width = ImGui::GetContentRegionAvail().x;
    const unsigned int height = ImGui::GetContentRegionAvail().y;

    if (m_bIsInit) {
        backend.PlayDirect(width, height);
        m_bIsInit = false;
        m_prevWidth = width;
        m_prevHeight = height;
    }

    if ((m_prevWidth != width || m_prevHeight != height)) {
        backend.ResizePreviewDirect(width, height);
        backend.InvokeEditorRender();
    }

    if (!backend.IsPlaying()) {
        if(ImGui::IsWindowFocused() || ImGui::IsWindowHovered()) {
            for (ImGuiKey key = static_cast<ImGuiKey>(0); key < ImGuiKey_COUNT; key = (ImGuiKey) (key + 1)) {
                if (ImGui::IsKeyDown(key)) {
                    backend.InvokeEditorRender();
                    break;
                }
            }
        }
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddImage(
            (void*) backend.GetPreviewTextureId(),
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
    EditorBackend::GetInstance().StopDirect();
    m_bIsInit = false;
}

bool PreviewWindow::IsPreview() const {
    return EditorBackend::GetInstance().IsPlaying();
}
