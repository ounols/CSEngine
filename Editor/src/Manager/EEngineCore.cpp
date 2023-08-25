#include "EEngineCore.h"
#include "EPreviewCore.h"
#include "../../src/MacroDef.h"
#include "../../src/OGLDef.h"
#include <chrono>

using namespace CSE;
using namespace CSEditor;

EEngineCore* EEngineCore::sInstance = nullptr;

EngineCoreInstance* EEngineCore::getInstance() {
    if (sInstance == nullptr) sInstance = new EEngineCore;
    if (sInstance->IsPreview()) return sInstance->m_previewCore;
    return sInstance;
}

EEngineCore* EEngineCore::getEditorInstance() {
    if (sInstance == nullptr) sInstance = new EEngineCore;
    return sInstance;
}

void EEngineCore::delInstance() {
    {
        delete (sInstance);
        { (sInstance) = nullptr; };
    };
}

EEngineCore::EEngineCore() = default;

EEngineCore::~EEngineCore() {
    glDeleteTextures(1, &m_previewTextureId);
    glDeleteFramebuffers(1, &m_previewFbo);
    if (m_previewCore != nullptr) StopPreviewCore();
}

void EEngineCore::BindPreviewFramebuffer() const {
    if (!IsPreview()) return;
    glBindFramebuffer(GL_FRAMEBUFFER, m_previewFbo);
}

void EEngineCore::InitPreviewFramebuffer() {
    glGenFramebuffers(1, &m_previewFbo);
    glGenTextures(1, &m_previewTextureId);
}

void EEngineCore::StartPreviewCore() {
    if (m_previewCore != nullptr) throw -1;

    m_previewElapsedTime = 0.f;
    m_startTime = GetCurrentMillis();

    m_previewCore = new EPreviewCore();
    if (m_previewFbo <= 0) {
        InitPreviewFramebuffer();
        BindPreviewFramebuffer();
        ResizePreviewFramebuffer(m_previewWidth, m_previewHeight);
    }
    m_previewCore->Init(m_previewWidth, m_previewHeight);
    m_previewCore->SetDeviceBuffer(m_previewFbo);
}

void EEngineCore::StopPreviewCore() {
    m_previewCore->Exterminate();
    delete m_previewCore;
    m_previewCore = nullptr;
    m_previewElapsedTime = 0.f;
    m_startTime = 0;
}

void EEngineCore::UpdatePreviewCore() {
    m_previewElapsedTime = GetCurrentMillis() - m_startTime;
    m_previewCore->Update(m_previewElapsedTime);
    m_previewCore->LateUpdate(m_previewElapsedTime);
}

void EEngineCore::RenderPreviewCore() const {
    m_previewCore->Render();
}

void EEngineCore::ResizePreviewCore() {
    ResizePreviewFramebuffer(m_previewWidth, m_previewHeight);
    m_previewCore->ResizeWindow(m_previewWidth, m_previewHeight);
}

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

long long int EEngineCore::GetCurrentMillis() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void EEngineCore::ResizePreviewFramebuffer(unsigned int width, unsigned int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_previewFbo);

    glBindTexture(GL_TEXTURE_2D, m_previewTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_previewTextureId, 0);
}
