#pragma once
#include "../Manager/Base/Scene.h"
#include "../Object/SGameObject.h"
#include "../Util/Render/MeshSurface.h"

class FirstDemoScene : public CSE::Scene {
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
	CSE::SGameObject* c;
	CSE::SGameObject* c2;	//new and delete
	CSE::SGameObject* c3;	//switching render
	CSE::SGameObject* d;
	//===============
	float startTIme = 0;
	CSE::MeshSurface* cube;
	CSE::MeshSurface* cubemap;

	bool isUnvisible = false;
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
};

