#include "CameraComponent.h"
#include "TransformComponent.h"
#include "../Manager/CameraMgr.h"

#include <mutex>

using namespace CSE;

COMPONENT_CONSTRUCTOR(CameraComponent), m_eye(nullptr), m_targetObject(nullptr) {
    CameraMgr::getInstance()->Register(this);
    m_pRatio = const_cast<float*>(CameraMgr::getInstance()->GetProjectionRatio());
}


CameraComponent::~CameraComponent() {

}


void CameraComponent::Exterminate() {
    CameraMgr::getInstance()->Remove(this);
}


void CameraComponent::Init() {
    m_eye = static_cast<TransformComponent*>(gameObject->GetTransform())->GetPosition();
    m_target = vec3(0, 0, -1);
    m_up = vec3(0, 1, 0);

    m_resultTarget = vec3();
    m_pRatio = const_cast<float*>(CameraMgr::getInstance()->GetProjectionRatio());
}


void CameraComponent::Tick(float elapsedTime) {

    if (m_targetObject == nullptr)
        m_resultTarget = *m_eye + m_target;
    else {
        m_resultTarget = *static_cast<TransformComponent*>(m_targetObject->GetTransform())->GetPosition();
    }

    SetCameraMatrix();

}

SComponent* CameraComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(CameraComponent, comp);

    comp->m_eye = m_eye;
    comp->m_target = m_target;
    comp->m_up = m_up;

    comp->m_cameraMatrix = m_cameraMatrix;
    comp->m_projectionMatrix = m_projectionMatrix;
    comp->m_resultTarget = m_resultTarget;


    comp->m_type = m_type;
    comp->m_isProjectionInited = m_isProjectionInited;

    //perspective
    comp->m_pFov = m_pFov;
    comp->m_pRatio = m_pRatio;

    //Ortho
    comp->m_oLeft = m_oLeft;
    comp->m_oRight = m_oRight;
    comp->m_oBottom = m_oBottom;
    comp->m_oTop = m_oTop;

    comp->m_Near = m_Near;
    comp->m_Far = m_Far;

    return comp;
}

void CameraComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                    std::map<SComponent*, SComponent*> lists_comp) {
    if (src == nullptr) return;
    CameraComponent* convert = static_cast<CameraComponent*>(src);

    //Copy GameObjects
    FIND_OBJ_REFERENCE(m_targetObject, convert);

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

mat4 CameraComponent::GetCameraMatrix() const {
    return m_cameraMatrix;
}

vec3 CameraComponent::GetCameraPosition() const {
    mat4 matrix = static_cast<TransformComponent*>(gameObject->GetTransform())->GetMatrix();
    return vec3{ matrix.w.x, matrix.w.y, matrix.w.z };
}


void CameraComponent::SetProjectionMatrix() {
	std::mutex mutex;

	mutex.lock();
    if (m_type == PERSPECTIVE) {
        m_projectionMatrix = mat4::Perspective(m_pFov, *m_pRatio, m_Near, m_Far);

    } else {
        m_projectionMatrix = mat4::Ortho(m_oLeft, m_oRight, m_oBottom, m_oTop, m_Near, m_Far);
    }

    m_isProjectionInited = true;
	mutex.unlock();
}

void CameraComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {

    if (name_str == "m_eye") {
        m_eye = gameObject->GetComponentByID<TransformComponent>(value[0])->GetPosition();
    } else if (name_str == "m_target") {
        SET_VEC3(m_target);
    } else if (name_str == "m_up") {
        SET_VEC3(m_up);
    } else if (name_str == "m_targetObject") {
        m_targetObject = gameObject->FindByID(value[0]);
    } else if (name_str == "m_cameraMatrix") {
        SET_MAT4(m_cameraMatrix);
    } else if (name_str == "m_projectionMatrix") {
        SET_MAT4(m_projectionMatrix);
    } else if (name_str == "m_type") {
        if (value[0] == "PERSPECTIVE") {
            m_type = PERSPECTIVE;
        } else {
            m_type = ORTHO;
        }
    } else if (name_str == "m_pFov") {
        m_pFov = std::stof(value[0]);
    } else if (name_str == "m_orthoValue") {
        m_oLeft = std::stof(value[0]);
        m_oRight = std::stof(value[1]);
        m_oBottom = std::stof(value[2]);
        m_oTop = std::stof(value[3]);

    } else if (name_str == "m_distance") {
        m_Near = std::stof(value[0]);
        m_Far = std::stof(value[1]);
    }

}

std::string CameraComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE(m_eye, ConvertSpaceStr(gameObject->GetID(gameObject->GetComponent<TransformComponent>())));
    PRINT_VALUE_VEC3(m_target);
    PRINT_VALUE_VEC3(m_up);
    PRINT_VALUE(m_targetObject, m_targetObject == nullptr ? "" : ConvertSpaceStr(m_targetObject->GetID()));
    PRINT_VALUE_MAT4(m_cameraMatrix);
    PRINT_VALUE_MAT4(m_projectionMatrix);
    PRINT_VALUE(m_type, m_type == PERSPECTIVE ? "PERSPECTIVE" : "ORTHO");
    PRINT_VALUE(m_pFov, m_pFov);
    PRINT_VALUE(m_orthoValue, m_oLeft, ' ', m_oRight, ' ', m_oBottom, ' ', m_oTop);
    PRINT_VALUE(m_distance, m_Near, ' ', m_Far);


    PRINT_END("component");
}
