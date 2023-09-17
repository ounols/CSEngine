#include "EngineCoreInstance.h"
#include "Base/CoreBase.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "OGLMgr.h"
#include "ScriptMgr.h"
#include "CameraMgr.h"
#include "LightMgr.h"
#include "Render/RenderMgr.h"
#include "SceneMgr.h"
#include "MemoryMgr.h"
#include "ReflectionMgr.h"
#include "../Sample/FirstDemoScene.h"
#include "../Sample/WebDemoScene.h"
#include "../Util/Loader/SCENE/SSceneLoader.h"
#include "../Util/AssetsDef.h"

#ifdef __EMSCRIPTEN__
#include "../Sample/WebDemoScene.h"
#endif

using namespace CSE;
constexpr short ENGINE_COUNT = 10;

EngineCoreInstance::EngineCoreInstance() {

}

EngineCoreInstance::~EngineCoreInstance() {

}

void EngineCoreInstance::Init(unsigned int width, unsigned int height) {
    GenerateCores();

    for (const auto& core : m_cores) {
        core->Init();
    }

    m_oglMgr->ResizeWindow(width, height);
    static_cast<RenderMgr*>(m_renderMgr)->SetViewport();


#ifdef __EMSCRIPTEN__
    static_cast<SceneMgr*>(m_sceneMgr)->SetScene(new WebDemoScene());
#else
//    static_cast<SceneMgr*>(m_sceneMgr)->SetScene(new FirstDemoScene());
    SScene* scene = SSceneLoader::LoadScene(CSE::AssetsPath() + "Scene/sdfgi.scene");
    m_sceneMgr->SetScene(scene);
#endif
    m_isReady = true;
}

void EngineCoreInstance::Update(float elapsedTime) {
    for (const auto& core : m_updateCores) {
        core->Update(elapsedTime);
    }
}

void EngineCoreInstance::LateUpdate(float elapsedTime) {
    m_gameObjectMgr->DestroyQueuedObject();
}

void EngineCoreInstance::Render() const {
    for (const auto& core : m_renderCores) {
        core->Render();
    }
}

void EngineCoreInstance::Exterminate() {
    ExterminateWithoutReflectionDefine();
    CSE::ReflectionMgr::DefineWrapper::ReleaseDefine();
}

void EngineCoreInstance::ExterminateWithoutReflectionDefine() {
    if(m_memoryMgr != nullptr)
        static_cast<MemoryMgr*>(m_memoryMgr)->ExterminateObjects(true);
    for (auto core : m_cores) {
        SAFE_DELETE(core);
    }
    m_cores.clear();
}

void EngineCoreInstance::GenerateCores() {
    if(m_isGenerated) return;
    m_isGenerated = true;
    m_cores = std::vector<CoreBase*>();
    m_cores.reserve(ENGINE_COUNT);

    m_reflectionMgr = new ReflectionMgr();
    m_resMgr = new ResMgr();
    m_gameObjectMgr = new GameObjectMgr();
    m_oglMgr = new OGLMgr();
    m_renderMgr = new RenderMgr();
    m_cameraMgr = new CameraMgr();
    m_lightMgr = new LightMgr();
    m_sceneMgr = new SceneMgr();
    m_memoryMgr = new MemoryMgr();
    m_scriptMgr = new ScriptMgr();

    m_cores.push_back(m_reflectionMgr);
    m_cores.push_back(m_resMgr);
    m_cores.push_back(m_gameObjectMgr);
//    m_updateCores.push_back(m_gameObjectMgr);

    m_cores.push_back(m_oglMgr);
    m_renderCores.push_back(m_oglMgr);
    m_cores.push_back(m_renderMgr);
    m_renderCores.push_back(dynamic_cast<RenderCoreBase* const>(m_renderMgr));
    m_cores.push_back(m_cameraMgr);
    m_cores.push_back(m_lightMgr);

    m_cores.push_back(m_sceneMgr);
    m_updateCores.push_back(m_sceneMgr);
    m_cores.push_back(m_memoryMgr);

    m_cores.push_back(m_scriptMgr);
}

void EngineCoreInstance::ResizeWindow(unsigned int width, unsigned int height) {
    m_oglMgr->ResizeWindow(width, height);
    static_cast<RenderMgr*>(m_renderMgr)->SetViewport();
}

void EngineCoreInstance::SetDeviceBuffer(unsigned int id) {
    static_cast<RenderMgr*>(m_renderMgr)->SetDeviceBuffer(id);
}
