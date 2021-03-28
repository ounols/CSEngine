#include "LightComponent.h"
#include "../Manager/LightMgr.h"
#include "TransformComponent.h"
#include "../Manager/EngineCore.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(LightComponent) {

    auto lightMgr = CORE->GetCore(LightMgr);
    lightMgr->Register(this);

    m_light = new SLight();

    SetLightType(DIRECTIONAL);
}


LightComponent::~LightComponent() {}


void LightComponent::Exterminate() {

    CORE->GetCore(LightMgr)->Remove(this);
    SAFE_DELETE(m_light);

}


void LightComponent::Init() {
}


void LightComponent::Tick(float elapsedTime) {
    if (m_isSunRising && m_type == DIRECTIONAL) {
        float value = m_light->direction.y;
        float bright = (value < 0.2) ? (value - 0.2f) * 2 + 1 : 1;
        if (bright < 0) bright = 0;
        if (value < 0) value = 0;

        float color0 = (0.4f * (1 - value) + 0.4f) * bright * 5;
        float color1 = (0.3f * value + 0.3f) * bright * 5;

//		float color2 = (value_str * 0.07f + 0.03f) * 10;

        m_light->color = vec3{ color0, color1, color1 };
    }
}


void LightComponent::SetLightType(LIGHT type) {

    m_type = type;


    if (m_type == POINT || m_type == SPOT) {
        SetLightPosition();
    }

}


void LightComponent::SetDirection(vec4 direction) const {

    switch (m_type) {

        case DIRECTIONAL:
        case SPOT:

            m_light->direction = direction;
            return;

        default:
            return;
    }

}


void LightComponent::SetColor(vec3 color) const {
    m_light->color = color;

}

void LightComponent::SetLightRadius(float radius) const {

    m_light->radius = radius;

}


void LightComponent::SetAttenuationFactor(vec3 att) const {

    m_light->att = att;

}


void LightComponent::SetAttenuationFactor(float Kc, float Kl, float Kq) const {

    SetAttenuationFactor(vec3{ Kc, Kl, Kq });

}


void LightComponent::SetLightPosition() const {

    m_light->position = static_cast<TransformComponent*>(gameObject->GetTransform())->GetPosition();

}

vec4 LightComponent::GetDirection(vec4 direction) const {
    return m_light->direction;
}

vec3 LightComponent::GetColor() const {
    return m_light->color;
}

void LightComponent::SetSunrising(bool active) {
    m_isSunRising = active;

}

SComponent* LightComponent::Clone(SGameObject* object) {
	INIT_COMPONENT_CLONE(LightComponent, clone);

	clone->m_type = m_type;
	clone->DisableAmbient = DisableAmbient;
	clone->DisableDiffuse = DisableDiffuse;
	clone->DisableSpecular = DisableSpecular;

	return clone;
}

void LightComponent::SetValue(std::string name_str, Arguments value) {
}

std::string LightComponent::PrintValue() const {
	PRINT_START("component");

	PRINT_VALUE(m_type, static_cast<int>(m_type));
	//PRINT_VALUE(DisableAmbient, m_startTime);
	//PRINT_VALUE(m_currentAnimation, ConvertSpaceStr(m_currentAnimation->GetID()));
	//PRINT_VALUE(m_entity, ConvertSpaceStr(m_entity->GetGameObject()->GetID(m_entity)));


	PRINT_END("component");
}
