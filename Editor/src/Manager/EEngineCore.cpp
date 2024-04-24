#include "EEngineCore.h"
#include "EPreviewCore.h"
#include "ELogMgr.h"
#include "../Objects/Base/HierarchyData.h"
#include "../../src/MacroDef.h"
#include "../../src/OGLDef.h"
#include "../../src/Util/Render/ShaderUtil.h"
#include "../Objects/ConsoleWindow.h"
#include <chrono>

#include "../../src/Manager/Base/CoreBase.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Manager/GameObjectMgr.h"
#include "../../src/Manager/OGLMgr.h"
#include "../../src/Manager/ScriptMgr.h"
#include "../../src/Manager/CameraMgr.h"
#include "../../src/Manager/LightMgr.h"
#include "../../src/Manager/Render/RenderMgr.h"
#include "../../src/Manager/SceneMgr.h"
#include "../../src/Util/Loader/SCENE/SSceneLoader.h"
#include "../../src/Manager/MemoryMgr.h"
#include "../../src/Manager/ReflectionMgr.h"
#include "../../src/Component/TransformComponent.h"

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
    if (m_previewCore != nullptr) StopPreviewCore();
    glDeleteTextures(1, &m_previewTextureId);
    glDeleteFramebuffers(1, &m_previewFbo);
}

void EEngineCore::BindPreviewFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_previewFbo);
}

void EEngineCore::InitPreviewFramebuffer() {
    glGenFramebuffers(1, &m_previewFbo);
    glGenTextures(1, &m_previewTextureId);
    SetDeviceBuffer(m_previewFbo);
}

void EEngineCore::StartPreviewCore() {
    if (m_previewCore != nullptr) throw -1;

    m_logMgr->ClearLog();
    m_previewElapsedTime = 0.f;
    m_startTime = GetCurrentMillis();

    m_previewCore = new EPreviewCore();
//    if (m_previewFbo <= 0) {
//        InitPreviewFramebuffer();
//        BindPreviewFramebuffer();
//        ResizePreviewFramebuffer(m_previewWidth, m_previewHeight);
//    }
    m_previewCore->Init(m_previewWidth, m_previewHeight);
    if(!m_scenePath.empty()) {
        SScene* scene = SSceneLoader::LoadScene(m_scenePath);
        m_previewCore->m_sceneMgr->SetScene(scene);
    }

    m_previewCore->SetDeviceBuffer(m_previewFbo);
}

void EEngineCore::StopPreviewCore() {
    m_previewCore->ExterminateWithoutReflectionDefine();
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
    if (IsPreview()) m_previewCore->ResizeWindow(m_previewWidth, m_previewHeight);
    else ResizeWindow(m_previewWidth, m_previewHeight);
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

void EEngineCore::GenerateCores() {
    if (m_isGenerated) return;
    m_isGenerated = true;
    m_cores = std::vector<CoreBase*>();
    m_cores.reserve(10);

    m_reflectionMgr = new ReflectionMgr();
    m_resMgr = new ResMgr();
    m_gameObjectMgr = new GameObjectMgr();
    m_oglMgr = new OGLMgr();
    m_renderMgr = new RenderMgr();
    m_cameraMgr = new CameraMgr();
    m_lightMgr = new LightMgr();
    m_sceneMgr = new SceneMgr();
    m_memoryMgr = new MemoryMgr();
    m_logMgr = new ELogMgr();

    m_cores.push_back(m_reflectionMgr);
    m_cores.push_back(m_resMgr);
    m_cores.push_back(m_gameObjectMgr);
    m_updateCores.push_back(m_gameObjectMgr);

    m_cores.push_back(m_oglMgr);
    m_renderCores.push_back(m_oglMgr);
    m_cores.push_back(m_renderMgr);
    m_renderCores.push_back(dynamic_cast<RenderCoreBase* const>(m_renderMgr));
    m_cores.push_back(m_cameraMgr);
    m_cores.push_back(m_lightMgr);

    m_cores.push_back(m_sceneMgr);
    m_updateCores.push_back(m_sceneMgr);
    m_cores.push_back(m_memoryMgr);

    m_cores.push_back(m_logMgr);
}

void EEngineCore::AddLog(const char* log, int category) {
    m_logMgr->AddLog(log, static_cast<ELogMgr::Category>(category));
}

void EEngineCore::UpdateTransforms() {
    const auto& objs = m_gameObjectMgr->GetAll();
    for (const auto& pair: objs) {
        const auto& transform = static_cast<TransformComponent*>(pair.second->GetTransform());
        transform->Tick(0);
    }
}

void EEngineCore::Reset() {
    m_hierarchyData->ClearSelectedObject();
}

void EEngineCore::SetCurrentScene(std::string path) {
    m_scenePath = std::move(path);
    const auto& scene = CSE::SSceneLoader::LoadScene(m_scenePath);
    m_sceneMgr->SetScene(scene);
}
