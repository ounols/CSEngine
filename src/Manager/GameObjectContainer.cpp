#include "GameObjectContainer.h"


GameObjectContainer::GameObjectContainer(): m_size(0) {
}


GameObjectContainer::~GameObjectContainer() {}


void GameObjectContainer::RegisterGameObject(SGameObject* object) {
	m_gameObjects.push_back(object);
	m_size++;
}
