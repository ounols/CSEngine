#include "EEngineCore.h"
#include "EPreviewCore.h"
#include "../../src/MacroDef.h"

using namespace CSE;
using namespace CSEditor;

IMPLEMENT_SINGLETON(EEngineCore)

EEngineCore::EEngineCore() = default;

EEngineCore::~EEngineCore() = default;

void EEngineCore::StartPreviewCore(unsigned int width, unsigned int height) {
    if(m_previewCore != nullptr) throw -1;

    m_previewCore = new EPreviewCore();
    m_previewCore->Init(width, height);
}

void EEngineCore::StopPreviewCore() {
    m_previewCore->Exterminate();
    delete m_previewCore;
    m_previewCore = nullptr;
}

void EEngineCore::UpdatePreviewCore(float elapsedTime) {
    m_previewCore->Update(elapsedTime);
    m_previewCore->LateUpdate(elapsedTime);
}

void EEngineCore::RenderPreviewCore() const {
    m_previewCore->Render();
}
