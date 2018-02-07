#include "MainProc.h"
#include "../Macrodef.h"
#include "MemoryMgr.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "RenderMgr.h"
#include "LightMgr.h"
#include "SceneMgr.h"


#include "../Sample/FirstDemoScene.h"


MainProc::MainProc()
{
	m_oglMgr = new OGLMgr();
}


MainProc::~MainProc()
{
	Exterminate();
}



void MainProc::Init(GLuint width, GLuint height) {

	m_oglMgr->setupEGLGraphics(width, height);
	GameObjectMgr::getInstance()->Init();

	SceneMgr::getInstance()->SetScene(new FirstDemoScene());


	
}


void MainProc::Update(float elapsedTime) {
	GameObjectMgr::getInstance()->Update(elapsedTime);
	SceneMgr::getInstance()->Tick(elapsedTime);

}


void MainProc::Render(float elapsedTime) const {

	m_oglMgr->Render(elapsedTime);
	RenderMgr::getInstance()->Render(elapsedTime);
}


void MainProc::Exterminate() {

	SAFE_DELETE(m_oglMgr);
	MemoryMgr::getInstance()->ExterminateObjects(true);

	ResMgr::delInstance();
	GameObjectMgr::delInstance();
	RenderMgr::delInstance();
	CameraMgr::delInstance();
	LightMgr::delInstance();
	SceneMgr::delInstance();

	MemoryMgr::delInstance();
	

}

void MainProc::ResizeWindow(GLuint width, GLuint height) const {

    m_oglMgr->ResizeWindow(width, height);

}
