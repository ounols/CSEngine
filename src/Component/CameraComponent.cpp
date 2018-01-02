#include "CameraComponent.h"
#include "TransformComponent.h"
#include "../Manager/CameraMgr.h"


CameraComponent::CameraComponent(): m_eye(nullptr), m_targetObject(nullptr) {
	RESMGR->Register<CameraMgr, CameraComponent>(this);
}


CameraComponent::~CameraComponent() {
	
}


void CameraComponent::Exterminate() {
}


void CameraComponent::Init() {
	m_eye = &Transform->m_position;
	m_target = vec3(0, 0, -1);
	m_up = vec3(0, 1, 0);

	m_resultTarget = vec3();
}


void CameraComponent::Tick(float elapsedTime) {

	if(m_targetObject == nullptr)
		m_resultTarget = *m_eye + m_target;

}


void CameraComponent::SetTarget(vec3 target) {

	m_target = target;

}


void CameraComponent::SetUp(vec3 up) {

	m_up = up;

}


void CameraComponent::SetCameraMatrix() {
	m_cameraMatrix = mat4::LookAt(*m_eye, m_resultTarget, m_up);
}
