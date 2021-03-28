#include "Scene.h"
#include "../MemoryMgr.h"
#include "../SceneMgr.h"
#include "../EngineCore.h"

using namespace CSE;

Scene::Scene() {
    SetUndestroyable(true);
}


Scene::~Scene() {

}


void Scene::Exterminate() {
    Destroy();
}


void Scene::SetScene(Scene* scene) {
    CORE->GetCore(SceneMgr)->SetScene(scene);
}
