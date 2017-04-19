#pragma once
#include "GameObjectContainer.h"
#include "../Macrodef.h"

class GameObjectMgr : public GameObjectContainer {
protected:
	DECLARE_SINGLETONE(GameObjectMgr);
	~GameObjectMgr();

public:
	void Update(float elapsedTime);
	void DeleteGameObject(SGameObject* object);

	SGameObject* Find(std::string name);

};
