#include "SceneMgr.h"
#include "MemoryMgr.h"
#include "EngineCore.h"
#include "Render/RenderMgr.h"

using namespace CSE;

SceneMgr::SceneMgr() = default;


SceneMgr::~SceneMgr() = default;


void SceneMgr::Init() {
    if (m_scene == nullptr) return;

    m_scene->Init();
}


void SceneMgr::Update(float elapsedTime) {

    if (m_scene == nullptr)
        return;

    m_scene->Tick(elapsedTime);

}


void SceneMgr::SetLegacyScene(Scene* scene) {

    if (m_scene != nullptr) {
        m_scene->SetUndestroyable(false);
        //MemoryMgr::getInstance()->ReleaseObject(m_scene);
        CORE->GetCore(MemoryMgr)->ExterminateObjects();
    }

    m_scene = scene;

    if (m_scene != nullptr)
        m_scene->Init();
}

void SceneMgr::SetScene(Scene* scene) {

    if (m_scene != nullptr) {
        m_scene->SetUndestroyable(false);
        CORE->GetCore(MemoryMgr)->ReleaseObject(m_scene);
        m_scene = nullptr;
    }

    m_scene = scene;

    if (m_scene != nullptr)
        m_scene->Init();

}

Scene* SceneMgr::GetCurrentScene() const {
    return m_scene;
}
