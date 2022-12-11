//
// Created by ounol on 2021-03-22.
//

#include "EngineCore.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "OGLMgr.h"
#include "ScriptMgr.h"
#include "CameraMgr.h"
#include "LightMgr.h"
#include "Render/RenderMgr.h"
#include "SceneMgr.h"
#include "MemoryMgr.h"
#include "../Sample/FirstDemoScene.h"
#include "../Sample/WebDemoScene.h"
#include "../Util/Loader/SCENE/SSceneLoader.h"
#include "../Util/AssetsDef.h"

#ifdef __EMSCRIPTEN__
#include "../Sample/WebDemoScene.h"
#endif

using namespace CSE;
IMPLEMENT_SINGLETON(EngineCore);

constexpr short ENGINE_COUNT = 9;

EngineCore::EngineCore() {
}

EngineCore::~EngineCore() {

}

void EngineCore::Init(unsigned int width, unsigned int height) {
    GenerateCores();

    for (const auto& core : m_cores) {
        core->Init();
    }

    m_oglMgr->ResizeWindow(width, height);
    static_cast<RenderMgr*>(m_renderMgr)->SetViewport();


#ifdef __EMSCRIPTEN__
    static_cast<SceneMgr*>(m_sceneMgr)->SetScene(new WebDemoScene());
#else
    static_cast<SceneMgr*>(m_sceneMgr)->SetScene(new FirstDemoScene());
//    SScene* scene = SSceneLoader::LoadScene(CSE::AssetsPath() + "Scene/reflection.scene");
//    m_sceneMgr->SetScene(scene);
#endif
}

void EngineCore::Update(float elapsedTime) {
    for (const auto& core : m_updateCores) {
        core->Update(elapsedTime);
    }
}

void EngineCore::LateUpdate(float elapsedTime) {
    m_gameObjectMgr->DestroyQueuedObject();
}

void EngineCore::Render() const {
    for (const auto& core : m_renderCores) {
        core->Render();
    }
}

void EngineCore::Exterminate() {
    static_cast<MemoryMgr*>(m_memoryMgr)->ExterminateObjects(true);
    for (auto core : m_cores) {
        SAFE_DELETE(core);
    }
    m_cores.clear();
}

void EngineCore::GenerateCores() {
    if(m_isGenerated) return;
    m_isGenerated = true;
    m_cores = std::vector<CoreBase*>();
    m_cores.reserve(ENGINE_COUNT);

    m_resMgr = new ResMgr();
    m_gameObjectMgr = new GameObjectMgr();
    m_oglMgr = new OGLMgr();
    m_renderMgr = new RenderMgr();
    m_cameraMgr = new CameraMgr();
    m_lightMgr = new LightMgr();
    m_sceneMgr = new SceneMgr();
    m_memoryMgr = new MemoryMgr();
    m_scriptMgr = new ScriptMgr();

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

    m_cores.push_back(m_scriptMgr);
}

void EngineCore::ResizeWindow(unsigned int width, unsigned int height) {
    m_oglMgr->ResizeWindow(width, height);
    static_cast<RenderMgr*>(m_renderMgr)->SetViewport();
}

