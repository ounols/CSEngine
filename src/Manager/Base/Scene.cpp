#include "Scene.h"
#include "../MemoryMgr.h"
#include "../SceneMgr.h"


Scene::Scene() {
	SetUndestroyable(true);
}


Scene::~Scene() {
	
}



void Scene::Exterminate() {
	Destroy();
}


void Scene::SetScene(Scene* scene) {
	SceneMgr::getInstance()->SetScene(scene);
}
