#include "WebDemoScene.h"
#include <iostream>
#include <sstream>
#include "../Util/Render/SEnvironmentMgr.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/CustomComponent.h"

#include "../Util/Loader/DAE/DAELoader.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../../Assets/cube.h"

using namespace CSE;

void WebDemoScene::Init() {
//    SPrefab* stormtrooper = SResource::Create<SPrefab>("stormtrooper.prefab");
    SPrefab* stormtrooper = SResource::Create<SPrefab>("model.prefab");

    auto root = new SGameObject("root");
    auto ab = stormtrooper->Clone(vec3{ 0, -0.3f, 0 }, root);
//    ab->GetTransform()->m_scale = vec3{ 0.2f, 0.2f, 0.2f };
    ab->GetTransform()->m_scale = vec3{ 0.08f, 0.08f, 0.08f };
    ab->GetTransform()->m_rotation = Quaternion::AngleAxis(vec3{1, 0, 0}, Pi/2.f);
    ab->CreateComponent<CustomComponent>();
    ab->GetComponent<CustomComponent>()->SetClassName("Stormtrooper.script");


    auto camera = new SGameObject("camera");
    camera->CreateComponent<CameraComponent>();
    camera->GetTransform()->m_position = vec3{0, 0, 1.2f };

    SGameObject* direction = new SGameObject();
    direction->SetParent(root);
    direction->SetName("directional");
    direction->GetTransform()->m_position = vec3{ 0.f, 0.5f, 0.f };
    direction->CreateComponent<LightComponent>();
    direction->GetComponent<LightComponent>()->SetSunrising(true);
    direction->GetComponent<LightComponent>()->SetLightType(LightComponent::DIRECTIONAL);
    direction->GetComponent<LightComponent>()->SetDirection(vec4{ 0.0f, 1.0f, 1, 0 });
    direction->GetComponent<LightComponent>()->SetShadow(true);


//    auto a_cam = camera->GetComponent<CameraComponent>();
//    a_cam->SetTarget(ab);

}

void WebDemoScene::Tick(float elapsedTime) {

}

void WebDemoScene::Destroy() {

}
