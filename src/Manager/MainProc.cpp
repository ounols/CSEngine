#include "MainProc.h"
#include "../Macrodef.h"
#include "MemoryMgr.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "../Component/DrawableStaticMeshComponent.h"


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

	////Managing Memory Test
	//SGameObject* a = new SGameObject();
	//SGameObject* b = new SGameObject();
	//SGameObject* c = new SGameObject();

	//b->Destroy();


}


void MainProc::Update(float elapsedTime) {
	GameObjectMgr::getInstance()->Update(elapsedTime);
}


void MainProc::Render(float elapsedTime) {

	m_oglMgr->Render(elapsedTime);

}


void MainProc::Exterminate() {

	SAFE_DELETE(m_oglMgr);
	ResMgr::delInstance();
	GameObjectMgr::delInstance();


	MemoryMgr::getInstance()->ExterminateObjects(true);
	MemoryMgr::delInstance();
	

}
