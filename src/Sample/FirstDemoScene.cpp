#include <iostream>
#include <sstream>
#include "FirstDemoScene.h"
#include "../Component/LightComponent.h"
#include "../Component/DrawableSkinnedMeshComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Assets/teapot_smooth.h"
#include "../../Assets/cube.h"
#include "../Component/MaterialComponent.h"
#include "../Component/RenderComponent.h"
#include "../Component/CustomComponent.h"
#include "../Manager/GameObjectMgr.h"

#include "../Util/Render/DAELoader.h"
#include "../Util/AssetsDef.h"
#include "../Manager/SCloneFactory.h"

FirstDemoScene::FirstDemoScene() {
	
}


FirstDemoScene::~FirstDemoScene() {
	
}


void FirstDemoScene::Init() {
	//===============

	//DAE test
	std::string path = CSE::AssetsPath() + "model.dae";
	DAELoader* daeLoader = new DAELoader(path.c_str(), nullptr, DAELoader::ALL);

//	daeLoader->GeneratePrefab();

	SGameObject* aa = new SGameObject();



	aa->CreateComponent<DrawableSkinnedMeshComponent>();
	aa->GetComponent<DrawableSkinnedMeshComponent>()->SetMesh(*(daeLoader->GetMesh()));
	aa->CreateComponent<MaterialComponent>();
	aa->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 0 });
	aa->GetComponent<MaterialComponent>()->SetShininess(40);

	aa->CreateComponent<RenderComponent>();
	aa->GetComponent<RenderComponent>()->SetShaderHandle(0);
	float scale = 0.07f;
	aa->GetTransform()->m_scale = vec3{scale, scale, scale};
	// aa->GetTransform()->m_rotation.x = 90.f;
	aa->GetTransform()->m_position.y = -0.3f;
	aa->CreateComponent<CustomComponent>();
	aa->GetComponent<CustomComponent>()->SetClassName("TestScript");
	aa->SetName("dae mesh");

	// cube = daeLoader->GetMesh();
	// cube->SetUndestroyable(false);
	


	//Managing Memory Test
	SGameObject* a = new SGameObject("camera");
	SGameObject* b = new SGameObject();
	d = new SGameObject();

	cube = new MeshSurface(CH02::teapot_smoothNumVerts, CH02::teapot_smoothVerts, CH02::teapot_smoothNormals);
	cube->SetUndestroyable(false);
	b->Destroy();
	c = new SGameObject();
	c->CreateComponent<DrawableStaticMeshComponent>();
	c->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
	c->CreateComponent<MaterialComponent>();
	c->GetComponent<MaterialComponent>()->SetShininess(40);

	c->CreateComponent<RenderComponent>();
	c->GetComponent<RenderComponent>()->SetShaderHandle(0);
	c->GetComponent<RenderComponent>()->SetIsEnable(false);
	c->CreateComponent<CustomComponent>();
	c->GetComponent<CustomComponent>()->SetClassName("TestScript");

	SGameObject* testing = new SGameObject("root");

	SGameObject* ab = daeLoader->GeneratePrefab()->Clone(vec3{1.f, -0.3f, 0.f}, testing);
	ab->GetTransform()->m_scale = vec3{ 0.1f, 0.1f, 0.1f };
	SAFE_DELETE(daeLoader);

	c2 = new SGameObject();
	// c2->AddComponent(c->GetComponent<DrawableStaticMeshComponent>());
	c2->CreateComponent<DrawableStaticMeshComponent>();
	c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
	c2->CreateComponent<MaterialComponent>();
	c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
	c2->GetTransform()->m_position.y = 0.7f;


	 
	c2->CreateComponent<RenderComponent>();
	c2->GetComponent<RenderComponent>()->SetShaderHandle(0);

	c3 = new SGameObject();
	c3->CreateComponent<DrawableStaticMeshComponent>();
	c3->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
	c3->CreateComponent<MaterialComponent>();
	c3->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 0 });
	c3->GetComponent<MaterialComponent>()->SetShininess(2);
	c3->GetTransform()->m_position.y = -0.7f;

	c3->CreateComponent<CustomComponent>();
	c3->GetComponent<CustomComponent>()->SetClassName("TestScript2");


	c3->CreateComponent<RenderComponent>();
	c3->GetComponent<RenderComponent>()->SetShaderHandle(0);

	auto c4 = SCloneFactory::Clone(c3, c3);
	c4->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 1 });
    c4->GetTransform()->m_position.y = -0.1f;
    c4->GetTransform()->m_position.x = -2.f;
	c4->GetTransform()->m_scale.Set(0.5, 0.5, 0.5);
	c4->DeleteComponent(c4->GetComponent<CustomComponent>());


	SGameObject* direction = new SGameObject();
	direction->SetName("directional");
	direction->GetTransform()->m_position = vec3{ 0.f, 1.f, 0.f };
	direction->CreateComponent<LightComponent>();
//	direction->GetComponent<LightComponent>()->SetColorAmbient(vec4{ 0.07f, 0.07f, 0.07f, 1 });
//	direction->GetComponent<LightComponent>()->SetColorDiffuse(vec4{ 0.3f, 0.5f, 0.5f, 1 });
	direction->GetComponent<LightComponent>()->DisableSpecular = false;
	direction->GetComponent<LightComponent>()->SetSunrising(true);
	direction->GetComponent<LightComponent>()->SetLightType(LightComponent::DIRECTIONAL);
	direction->GetComponent<LightComponent>()->SetDirection(vec4{ 0.0f, 1.0f, 0, 0 });
	direction->CreateComponent<CustomComponent>();
	direction->GetComponent<CustomComponent>()->SetClassName("directionalLight");

	d->SetName("light");
	d->CreateComponent<LightComponent>();
	d->GetTransform()->m_position = vec3{ 0.f, 5.f, -8.f };
	d->GetTransform()->m_scale = vec3{ 4.f, 4.f, 4.f };
	aa->AddChild(d);
	d->GetComponent<LightComponent>()->SetDirection(vec4{ 1.f, 0.5f, 1.f, 1.0f });
	d->GetComponent<LightComponent>()->SetColorAmbient(vec4{ 0.0f, 0.0f, 0.0f, 1 });
	d->GetComponent<LightComponent>()->DisableSpecular = false;
	d->GetComponent<LightComponent>()->SetLightType(LightComponent::POINT);
	d->GetComponent<LightComponent>()->SetLightRadius(1);
	d->CreateComponent<DrawableStaticMeshComponent>();
	d->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
	d->CreateComponent<MaterialComponent>();
	d->GetComponent<MaterialComponent>()->SetDiffuseMaterial(vec4{ 1, 1, 1, 1 });
	d->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 1 });
	d->CreateComponent<RenderComponent>();
	d->GetComponent<RenderComponent>()->SetShaderHandle(0);
	//d->GetComponent<LightComponent>()->DisableDiffuse = true;

	a->CreateComponent<CameraComponent>();
	a->GetTransform()->m_position = vec3{ 0, 0, 3.f };
	a->GetComponent<CameraComponent>()->SetTarget(ab);
	//===============
}


void FirstDemoScene::Tick(float elapsedTime) {

	if(startTIme == 0) {
		startTIme = elapsedTime;
	}

	//===============
//	SGameObject* d = GameObjectMgr::getInstance()->Find("dae mesh");
//	d->GetTransform()->m_position.x = sinf(elapsedTime*0.001) * 1.f;
//	d->GetTransform()->m_position.z = cosf(elapsedTime*0.001) * 1.f;
	//c->GetTransform()->m_rotation.y += 0.1f;
	c->GetTransform()->m_position.y = sinf(elapsedTime*0.001) * 0.1f;
	//===============

	if(elapsedTime - startTIme > 3000) {
		startTIme = elapsedTime;
		isUnvisible = !isUnvisible;
//		switchingObject();
	}
}


void FirstDemoScene::Destroy() {
}


void FirstDemoScene::switchingObject() {

	c3->SetIsEnable(!isUnvisible);

	if(isUnvisible) {
		c2->Destroy();
//        d->GetComponent<LightComponent>()->SetLightType(LightComponent::POINT);

    }else {
		c2 = new SGameObject();
		c2->CreateComponent<DrawableStaticMeshComponent>();
		c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
		c2->CreateComponent<MaterialComponent>();
		c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
		c2->GetComponent<MaterialComponent>()->SetMaterialSpecular(vec3{ 0, 0, 0 });
		c2->GetTransform()->m_position.y = 0.5f;


		c2->CreateComponent<RenderComponent>();
		c2->GetComponent<RenderComponent>()->SetShaderHandle(0);

//        d->GetComponent<LightComponent>()->SetLightType(LightComponent::DIRECTIONAL);
	}

}
