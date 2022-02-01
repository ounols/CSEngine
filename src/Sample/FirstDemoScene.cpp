#include "../Util/Render/SEnvironmentMgr.h"
#include "FirstDemoScene.h"
#include "../Component/LightComponent.h"
#include "../Component/DrawableSkinnedMeshComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Assets/teapot_smooth.h"
#include "../Component/RenderComponent.h"
#include "../Component/CustomComponent.h"
#include "../Manager/GameObjectMgr.h"

#include "../Util/Loader/DAE/DAELoader.h"
#include "../Util/AssetsDef.h"
#include "../Manager/SCloneFactory.h"
#include "../Util/Loader/SCENE/SSceneLoader.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/SMaterial.h"

using namespace CSE;

FirstDemoScene::FirstDemoScene() = default;


FirstDemoScene::~FirstDemoScene() = default;


void FirstDemoScene::Init() {
    //===============

    //DAE test
//    SPrefab* stormtrooper = MeshLoader::LoadModel("stormtrooper.model");
    SPrefab* stormtrooper = SResource::Create<SPrefab>("stormtrooper.prefab");
    SPrefab* plane = SResource::Create<SPrefab>("plane_circle.prefab");

//	daeLoader->GeneratePrefab();

//	SGameObject* aa = new SGameObject();
//
//
//
//	aa->CreateComponent<DrawableStaticMeshComponent>();
//	aa->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*(daeLoader->GetMesh()));
//	aa->CreateComponent<MaterialComponent>();
//	aa->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 0 });
//	aa->GetComponent<MaterialComponent>()->SetShininess(40);
//
//	aa->CreateComponent<RenderComponent>();
//	aa->GetComponent<RenderComponent>()->SetShaderHandle(0);
//	float scale = 0.1f;
//	aa->GetTransform()->m_scale = vec3{scale, scale, scale};
    // aa->GetTransform()->m_rotation.x = 90.f;
//	aa->GetTransform()->m_position.y = -0.3f;
//	aa->CreateComponent<CustomComponent>();
//	aa->GetComponent<CustomComponent>()->SetClassName("TestScript");
//	aa->SetName("dae mesh");
//	aa->GetComponent<RenderComponent>()->SetIsEnable(false);

    // cube = daeLoader->GetMesh();
    // cube->SetUndestroyable(false);


    SGameObject* root = new SGameObject("__ROOT_OF_SCENE__AHA___");

    //Managing Memory Test
    SGameObject* a = new SGameObject("camera");
    a->SetParent(root);
    SGameObject* b = new SGameObject();
    b->SetParent(root);
    d = new SGameObject();
    d->SetParent(root);
//	STexture* empty = new STexture();
//	empty->LoadEmpty();

    cube = new MeshSurface(CH02::teapot_smoothNumVerts, CH02::teapot_smoothVerts, CH02::teapot_smoothNormals);
    cube->SetUndestroyable(false);
    SFrameBuffer* buffer = new SFrameBuffer();
    buffer->GenerateFramebuffer(SFrameBuffer::PLANE, 512, 512);
    const auto& buf_tex = buffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGB);
    buf_tex->SetName("framebuffer_test");
    buf_tex->SetID("framebuffer_test");
    buffer->RasterizeFramebuffer();
    b->Destroy();
    c = new SGameObject("c");
    c->SetParent(root);
    c->CreateComponent<DrawableStaticMeshComponent>();
    c->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
//	c->GetComponent<MaterialComponent>()->SetShininess(40);
//	c->GetComponent<MaterialComponent>()->SetAlbedoTexture(empty);

    auto c_r = c->CreateComponent<RenderComponent>();
    c_r->GetMaterial()->SetVec3("FLOAT_ALBEDO", vec3{ 1, 0, 0 });
//    c->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");
    c->GetComponent<RenderComponent>()->SetIsEnable(false);
    c->CreateComponent<CustomComponent>();
    c->GetComponent<CustomComponent>()->SetClassName("testScript.script");

    SGameObject* testing = new SGameObject("root of mesh");
    root->AddChild(testing);

    SGameObject* ab = stormtrooper->Clone(vec3{ 0, -0.4f, 0 }, testing);
    SGameObject* ab2 = plane->Clone(vec3{ 0.5f, 0, 0 }, testing);
    ab->GetTransform()->m_scale = vec3{ 0.2f, 0.2f, 0.2f };
    ab2->GetTransform()->m_scale = vec3{ 0.2f, 0.2f, 0.2f };
    ab2->GetTransform()->m_rotation.Rotate(Quaternion::AngleAxis(vec3{0, 0, 1}, 3.14f));
    ab->CreateComponent<CustomComponent>();
    ab->GetComponent<CustomComponent>()->SetClassName("testScript.script");
//    auto ab_m = ab->GetComponent<RenderComponent>()->GetMaterial();

    const auto& ab2_children = ab2->GetChildren();
    for (const auto& gameObject : ab2_children) {
        const auto& component = gameObject->GetComponent<RenderComponent>();
        if(component == nullptr) continue;
        component->SetMaterial(SResource::Create<SMaterial>("File:Material/ForFramebufferTest.mat"));
    }

    c2 = new SGameObject();
    c2->SetParent(root);
    // c2->AddComponent(c->GetComponent<DrawableStaticMeshComponent>());
    c2->CreateComponent<DrawableStaticMeshComponent>();
    c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
//	c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
//	c2->GetComponent<MaterialComponent>()->SetAlbedoTexture(empty);
    c2->GetTransform()->m_position.y = 0.7f;


    c2->CreateComponent<RenderComponent>();
//    c2->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");

    c3 = new SGameObject("testscript2 object");
    c3->SetParent(root);
    c3->CreateComponent<DrawableStaticMeshComponent>();
    c3->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
//	c3->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 0 });
//	c3->GetComponent<MaterialComponent>()->SetShininess(2);
//	c3->GetComponent<MaterialComponent>()->SetAlbedoTexture(empty);
    c3->GetTransform()->m_position.y = -0.7f;

    c3->CreateComponent<CustomComponent>();
    c3->GetComponent<CustomComponent>()->SetClassName("testScript2.script");
//    {
//		std::vector<std::string> variable = {
//		"materialComp", c3->GetID(c3->GetComponent<MaterialComponent>()), "comp"
//		};
//		c3->GetComponent<CustomComponent>()->SetValue("__variable__", variable);
//    }
//	{
//		std::vector<std::string> variable = {
//		"testScript", c->GetID(c->GetComponent<CustomComponent>()), "nut"
//		};
//		c3->GetComponent<CustomComponent>()->SetValue("__variable__", variable);
//	}
	


    c3->CreateComponent<RenderComponent>();
//    c3->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");

    auto c4 = SCloneFactory::Clone(c3, c3);
//	c4->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 1 });
//	c4->GetComponent<MaterialComponent>()->SetAlbedoTexture(empty);
    c4->GetTransform()->m_position.y = -0.1f;
    c4->GetTransform()->m_position.x = -2.f;
    c4->GetTransform()->m_scale.Set(0.5, 0.5, 0.5);
    c4->DeleteComponent(c4->GetComponent<CustomComponent>());




    SGameObject* direction = new SGameObject();
    direction->SetParent(root);
    direction->SetName("directional");
    direction->GetTransform()->m_position = vec3{ 0.f, 1.f, 0.f };
    direction->CreateComponent<LightComponent>();
//	direction->GetComponent<LightComponent>()->SetColorAmbient(vec4{ 0.07f, 0.07f, 0.07f, 1 });
//	direction->GetComponent<LightComponent>()->SetColorDiffuse(vec4{ 0.3f, 0.5f, 0.5f, 1 });
    direction->GetComponent<LightComponent>()->SetSunrising(true);
    direction->GetComponent<LightComponent>()->SetLightType(LightComponent::DIRECTIONAL);
    direction->GetComponent<LightComponent>()->SetDirection(vec4{ 0.0f, 1.0f, 1, 0 });
    direction->GetComponent<LightComponent>()->SetShadow(true);

    direction->CreateComponent<CustomComponent>();
    direction->GetComponent<CustomComponent>()->SetClassName("directionalLight.script");
//
//
//    SComponent* test_comp = direction->GetComponent<CustomComponent>();
//
//    auto auto_comp = test_comp->GetComponent();

    d->SetName("light");
    d->CreateComponent<LightComponent>();
    d->GetTransform()->m_position = vec3{ 2.f, 2.f, -2.f };
    d->GetTransform()->m_scale = vec3{ 2.f, 2.f, 2.f };
    ab->AddChild(d);
    d->GetComponent<LightComponent>()->SetDirection(vec4{ 1.f, 0.5f, 1.f, 1.0f });
    d->GetComponent<LightComponent>()->SetColor(vec3{ 10, 10, 10 });
    d->GetComponent<LightComponent>()->SetLightType(LightComponent::POINT);
    d->GetComponent<LightComponent>()->SetLightRadius(1);
    d->GetComponent<LightComponent>()->SetShadow(false);
    d->CreateComponent<DrawableStaticMeshComponent>();
    d->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
//    d->CreateComponent<MaterialComponent>();
//    d->GetComponent<MaterialComponent>()->AttachMaterials("File:Material/DefaultPBR.mat");
//	d->GetComponent<MaterialComponent>()->SetDiffuseMaterial(vec4{ 1, 1, 1, 1 });
//    d->GetComponent<MaterialComponent>()->SetAlbedo(vec3{ 1, 1, 1 });
//	d->GetComponent<MaterialComponent>()->SetAlbedoTexture(empty);
    auto d_r = d->CreateComponent<RenderComponent>();
    d_r->GetMaterial()->SetVec3("FLOAT_ALBEDO", vec3{ 0, 1, 0 });
//    d->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");
    //d->GetComponent<LightComponent>()->DisableDiffuse = true;

    a->CreateComponent<CameraComponent>();
//    a->GetComponent<CameraComponent>()->SetOrtho(-3.f, 3.f, -3.f, 3.f);
//    a->GetComponent<CameraComponent>()->SetCameraType(CSE::CameraComponent::ORTHO);
    a->GetTransform()->m_position = vec3{ 0, 0, 3.f };
//    a->GetTransform()->m_position = vec3{ 0.f, 2.f, 4.f };
//    a->GetTransform()->m_scale = vec3{ 2.f, 2.f, 2.f };
//    a->GetComponent<CameraComponent>()->SetTarget(vec3{ 0.0f, -1.0f, -1.f });
    auto a_cam = a->GetComponent<CameraComponent>();
    a_cam->SetTarget(d);

    c4->CreateComponent<CameraComponent>();
    c4->GetComponent<CameraComponent>()->SetTarget(d);
    c4->GetComponent<CameraComponent>()->SetFrameBuffer(SResource::Create<SFrameBuffer>("File:Texture/TestFrameBuffer.framebuffer"));
    //===============

    SSceneLoader::SavePrefab(root, CSE::NativeAssetsPath() + "Scene/test_scene.scene");
}


void FirstDemoScene::Tick(float elapsedTime) {

    if (startTIme == 0) {
        startTIme = elapsedTime;
    }

    //===============
//	SGameObject* d = GameObjectMgr::getInstance()->Find("dae mesh");
//	d->GetTransform()->m_position.x = sinf(elapsedTime*0.001) * 1.f;
//	d->GetTransform()->m_position.z = cosf(elapsedTime*0.001) * 1.f;
    //c->GetTransform()->m_rotation.y += 0.1f;
    c->GetTransform()->m_position.y = sinf(elapsedTime * 0.001) * 0.1f;
    //===============

    if (elapsedTime - startTIme > 3000) {
        startTIme = elapsedTime;
        isInvisible = !isInvisible;
//		switchingObject();
    }
}


void FirstDemoScene::Destroy() {
}


void FirstDemoScene::switchingObject() {

    c3->SetIsEnable(!isInvisible);

    if (isInvisible) {
        c2->Destroy();
//        d->GetComponent<LightComponent>()->SetLightType(LightComponent::POINT);

    } else {
        c2 = new SGameObject();
        c2->CreateComponent<DrawableStaticMeshComponent>();
        c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube);
//        c2->CreateComponent<MaterialComponent>();
//		c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
//		c2->GetComponent<MaterialComponent>()->SetMaterialSpecular(vec3{ 0, 0, 0 });
        c2->GetTransform()->m_position.y = 0.5f;


        c2->CreateComponent<RenderComponent>();
//        c2->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");

//        d->GetComponent<LightComponent>()->SetLightType(LightComponent::DIRECTIONAL);
    }

}
