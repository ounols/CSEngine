#pragma once
#include <vector>
#include "../Object/SGameObject.h"

class GameObjectContainer {
protected:
	GameObjectContainer();
	~GameObjectContainer();

public:
	void RegisterGameObject(SGameObject* object);

protected:
	std::vector<SGameObject*> m_gameObjects;
	int m_size;
};

