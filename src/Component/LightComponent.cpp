#include "LightComponent.h"
#include "../Manager/LightMgr.h"


LightComponent::LightComponent() {
	
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
		
	case LIGHT::DIRECTIONAL:
	case LIGHT::SPOT:

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


void LightComponent::SetLightPosition() const {

	m_light->position = &Transform->m_position;

}
