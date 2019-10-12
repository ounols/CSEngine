//
// Created by ounols on 19. 8. 31.
//

#include "MoreComponentFunc.h"
#include "../Component/Animation/AnimatorComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/CustomComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/MaterialComponent.h"
#include "../Component/RenderComponent.h"
#include "../Component/TransformComponent.h"

using namespace CSE;

SComponent* MoreComponentFunc::CreateComponent(SGameObject* obj, std::string component_type) {

    SComponent* comp = nullptr;

    if (component_type == "AnimatorComponent") {
        comp = obj->CreateComponent<AnimatorComponent>();
    } else if (component_type == "JointComponent") {
        comp = obj->CreateComponent<JointComponent>();
    } else if (component_type == "CameraComponent") {
        comp = obj->CreateComponent<CameraComponent>();
    } else if (component_type == "CustomComponent") {
        comp = obj->CreateComponent<CustomComponent>();
    } else if (component_type == "DrawableSkinnedMeshComponent") {
        comp = obj->CreateComponent<DrawableSkinnedMeshComponent>();
    } else if (component_type == "DrawableStaticMeshComponent") {
        comp = obj->CreateComponent<DrawableStaticMeshComponent>();
    } else if (component_type == "LightComponent") {
        comp = obj->CreateComponent<LightComponent>();
    } else if (component_type == "MaterialComponent") {
        comp = obj->CreateComponent<MaterialComponent>();
    } else if (component_type == "RenderComponent") {
        comp = obj->CreateComponent<RenderComponent>();
    } else if (component_type == "TransformComponent") {
        comp = obj->GetComponent<TransformComponent>();
    }


    return comp;

}