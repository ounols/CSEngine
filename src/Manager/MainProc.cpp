#include "MainProc.h"
#include "../Macrodef.h"
#include "MemoryMgr.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "RenderMgr.h"
#include "../../Assets/cube.h"
#include "../Component/DrawableStaticMeshComponent.h"
#include "../Component/RenderComponent.h"
#include "../Component/TransformComponent.h"


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

	//ObjSurface* cube = new ObjSurface(CH02::teapot2NumVerts, CH02::teapot2Verts, CH02::teapot2Normals);

	//b->Destroy();

	//c->CreateComponent<DrawableStaticMeshComponent>();
	//c->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);

	//c->CreateComponent<RenderComponent>();
	//c->GetComponent<RenderComponent>()->SetShaderHandle(0);
	//c->GetComponent<TransformComponent>()->m_position.y = 1.f;

	GameObjectMgr::getInstance()->Init();
}


void MainProc::Update(float elapsedTime) {
	GameObjectMgr::getInstance()->Update(elapsedTime);
}


void MainProc::Render(float elapsedTime) const {

	m_oglMgr->Render(elapsedTime);
	RenderMgr::getInstance()->Render(elapsedTime);
}


void MainProc::Exterminate() {

	SAFE_DELETE(m_oglMgr);
	ResMgr::delInstance();
	GameObjectMgr::delInstance();
	RenderMgr::delInstance();

	MemoryMgr::getInstance()->ExterminateObjects(true);
	MemoryMgr::delInstance();
	

}
