#pragma once
#include <vector>
#include "../Object/SGameObject.h"
#include "Base/SContainer.h"

class GameObjectContainer : public SContainer<SGameObject*> {
protected:
	GameObjectContainer();
	~GameObjectContainer();

};

