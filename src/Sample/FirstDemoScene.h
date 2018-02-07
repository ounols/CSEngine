#pragma once
#include "../Manager/Base/Scene.h"
#include "../Object/SGameObject.h"
#include "../Util/Render/ObjSurface.h"

class FirstDemoScene : public Scene {
public:
	FirstDemoScene();
	~FirstDemoScene();

	void Init() override;
	void Tick(float elapsedTime) override;
	void Destroy() override;

private:
	void switchingObject();

private:
	//===============
	SGameObject* c;
	SGameObject* c2;	//new and delete
	SGameObject* c3;	//switching render
	//===============
	float startTIme = 0;
	ObjSurface* cube;

	bool isUnvisible = false;
};

