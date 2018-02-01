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
#include "LightMgr.h"


MainProc::MainProc()
{
	m_oglMgr = new OGLMgr();
}


MainProc::~MainProc()
{
	Exterminate();
}

//===============
SGameObject* c = new SGameObject();
//===============

void MainProc::Init(GLuint width, GLuint height) {

	m_oglMgr->setupEGLGraphics(width, height);
	GameObjectMgr::getInstance()->Init();

	//===============
	//Managing Memory Test
	SGameObject* a = new SGameObject();
	SGameObject* b = new SGameObject();
	SGameObject* d = new SGameObject();

	ObjSurface* cube = new ObjSurface(CH02::teapot2NumVerts, CH02::teapot2Verts, CH02::teapot2Normals);

	b->Destroy();

	c->CreateComponent<DrawableStaticMeshComponent>();
	c->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);

	c->CreateComponent<RenderComponent>();
	c->GetComponent<RenderComponent>()->SetShaderHandle(0);
	//c->GetTransform()->m_position.y = 0.5f;

	a->CreateComponent<CameraComponent>();
	a->GetTransform()->m_position = vec3{ 0, 0, 0.5f };
	a->GetComponent<CameraComponent>()->SetTarget(c);

	d->CreateComponent<LightComponent>();
	d->GetTransform()->m_position = vec3{ 0, 0.3f, 0 };
	d->GetComponent<LightComponent>()->SetDirection(vec4{ 6.f, 0.f, -6.f, 1.0f });
	//===============

	
}


void MainProc::Update(float elapsedTime) {
	GameObjectMgr::getInstance()->Update(elapsedTime);

	//===============
	c->GetTransform()->m_rotation.y += 0.1f;
	c->GetTransform()->m_position.y = sinf(elapsedTime*0.001) * 0.1f;
	//===============

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
	CameraMgr::delInstance();
	LightMgr::delInstance();

	MemoryMgr::getInstance()->ExterminateObjects(true);
	MemoryMgr::delInstance();
	

}
