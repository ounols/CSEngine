#include "CameraComponent.h"
#include "TransformComponent.h"
#include "../Manager/CameraMgr.h"


COMPONENT_CONSTRUCTOR(CameraComponent), m_eye(nullptr), m_targetObject(nullptr) {
	CameraMgr::getInstance()->Register(this);
	m_pRatio = CameraMgr::getInstance()->GetProjectionRatio();
}


CameraComponent::~CameraComponent() {
	
}


void CameraComponent::Exterminate() {
	CameraMgr::getInstance()->Remove(this);
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
	else {
		m_resultTarget = m_targetObject->GetTransform()->m_position;
	}

	SetCameraMatrix();

}


void CameraComponent::SetTarget(vec3 target) {

	m_target = target;

}


void CameraComponent::SetTarget(SGameObject* gameobject) {
	m_targetObject = gameobject;
}


void CameraComponent::SetUp(vec3 up) {

	m_up = up;

}


void CameraComponent::SetCameraType(CAMERATYPE type) {
	m_type = type;
}


void CameraComponent::SetPerspectiveFov(float fov) {
	m_pFov = fov;
}


void CameraComponent::SetZDepthRange(float near, float far) {
	m_Near = near;
	m_Far = far;

}


void CameraComponent::SetPerspective(float fov, float near, float far) {
	m_pFov = fov;
	m_Near = near;
	m_Far = far;

}


void CameraComponent::SetOrtho(float left, float right, float top, float bottom) {
	m_oLeft = left;
	m_oRight = right;
	m_oBottom = bottom;
	m_oTop = top;
}


void CameraComponent::SetCameraMatrix() {
	m_cameraMatrix = mat4::LookAt(*m_eye, m_resultTarget, m_up);
}


void CameraComponent::SetProjectionMatrix() {

	if(m_type == PERSPECTIVE) {
		m_projectionMatrix = mat4::Perspective(m_pFov, *m_pRatio, m_Near, m_Far);
		
	}else {
		m_projectionMatrix = mat4::Ortho(m_oLeft, m_oRight, m_oBottom, m_oTop, m_Near, m_Far);

	}

	m_isProjectionInited = true;

}
