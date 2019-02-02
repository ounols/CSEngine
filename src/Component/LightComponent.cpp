#include "LightComponent.h"
#include "../Manager/LightMgr.h"
#include "TransformComponent.h"


COMPONENT_CONSTRUCTOR(LightComponent) {
	
	LightMgr::getInstance()->Register(this);
}


LightComponent::~LightComponent() {}


void LightComponent::Exterminate() {

	LightMgr::getInstance()->Remove(this);
	SAFE_DELETE(m_light);

}


void LightComponent::Init() {
	m_light = new SLight();

	SetLightType(DIRECTIONAL);
}


void LightComponent::Tick(float elapsedTime) {
}


void LightComponent::SetLightType(LIGHT type) {

	m_type = type;


	if(m_type == POINT || m_type == SPOT) {
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


void LightComponent::SetColorAmbient(vec4 color) const {
	m_light->ambientColor = color;

}


void LightComponent::SetColorDiffuse(vec4 color) const {
	m_light->diffuseColor = color;

}


void LightComponent::SetColorSpecular(vec4 color) const {
	m_light->specularColor = color;

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
