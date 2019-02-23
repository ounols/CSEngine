#include "SceneMgr.h"
#include "MemoryMgr.h"

IMPLEMENT_SINGLETON(SceneMgr);

SceneMgr::SceneMgr() {
	
}


SceneMgr::~SceneMgr() {
	
}


void SceneMgr::Init() const {
	if (m_scene == nullptr) return;

	m_scene->Init();
}


void SceneMgr::Tick(float elapsedTime) const {

	if (m_scene == nullptr)
		return;

	m_scene->Tick(elapsedTime);

}


void SceneMgr::SetScene(Scene* scene) {

	if (m_scene != nullptr) {
		m_scene->SetUndestroyable(false);
		//MemoryMgr::getInstance()->ReleaseObject(m_scene);
		MemoryMgr::getInstance()->ExterminateObjects();
	}

	m_scene = scene;

	if(m_scene != nullptr)
		m_scene->Init();
}

Scene* SceneMgr::GetCurrentScene() const {
	return m_scene;
}
