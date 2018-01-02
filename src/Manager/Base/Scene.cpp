#include "Scene.h"
#include "../MemoryMgr.h"


Scene::Scene() {
}


Scene::~Scene() {
	MemoryMgr::getInstance()->ExterminateObjects();
}
