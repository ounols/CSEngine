#pragma once
#include "../Macrodef.h"
#include "GameObjectContainer.h"

class SGameObject;

class GameObjectMgr : public GameObjectContainer {
protected:
	DECLARE_SINGLETONE(GameObjectMgr);
	~GameObjectMgr();

public:
	void Init();
	void Update(float elapsedTime);
	void DeleteGameObject(SGameObject* object);

	SGameObject* Find(std::string name);

};
