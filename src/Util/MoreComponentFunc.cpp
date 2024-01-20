//
// Created by ounols on 19. 8. 31.
//

#include "MoreComponentFunc.h"
#include "../Component/Animation/AnimatorComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/CustomComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/RenderComponent.h"
#include "../Component/TransformComponent.h"

#define BIND_COMPONENT_MACRO(CLASSNAME) \
if(component_type == #CLASSNAME) { \
    auto comp_r = static_cast<CLASSNAME*>(component);   \
    instance->set(name.c_str(), comp_r);    \
    return; \
}

using namespace CSE;

void MoreComponentFunc::BindComponentToSQInstance(SComponent* component, std::string name,
                                                  sqext::SQIClassInstance* instance) {
    std::string component_type = component->GetClassType();

    BIND_COMPONENT_MACRO(AnimatorComponent);
    BIND_COMPONENT_MACRO(JointComponent);
    BIND_COMPONENT_MACRO(CameraComponent);
    BIND_COMPONENT_MACRO(CustomComponent);
    BIND_COMPONENT_MACRO(DrawableSkinnedMeshComponent);
    BIND_COMPONENT_MACRO(DrawableStaticMeshComponent);
    BIND_COMPONENT_MACRO(LightComponent);
    BIND_COMPONENT_MACRO(RenderComponent);
    BIND_COMPONENT_MACRO(TransformComponent);
}

