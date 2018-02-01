#include "LightComponent.h"
#include "../Manager/LightMgr.h"


LightComponent::LightComponent() {
	
	LightMgr::getInstance()->Register(this);
}


LightComponent::~LightComponent() {}


void LightComponent::Exterminate() {

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
		m_light->is_directional = false;
	}else {
		m_light->is_directional = true;
	}

}


void LightComponent::SetDirection(vec4 direction) {

	switch (m_type) {
		
	case LIGHT::DIRECTIONAL:
	case LIGHT::SPOT:

		m_light->direction = direction;
		return;

	default:
		return;
	}

}


void LightComponent::SetLightPosition() {

	m_light->position = &Transform->m_position;

}
