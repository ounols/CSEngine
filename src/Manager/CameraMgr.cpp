#include "CameraMgr.h"

using namespace CSE;

CameraMgr::CameraMgr() {
}


CameraMgr::~CameraMgr() {}


void CameraMgr::Init() {
}

void CameraMgr::DeleteCameraComponent(CameraComponent* object) {
    if (object == nullptr) return;

    auto iGameObj = std::find(m_objects.begin(), m_objects.end(), object);

    if (iGameObj != m_objects.end()) {
        m_size--;
        m_objects.erase(iGameObj);
        object->GetGameObject()->DeleteComponent(object);
        //MemoryMgr::getInstance()->ReleaseObject(object);
    }
}


const float* CameraMgr::GetProjectionRatio() const {
    return &m_projectionRatio;
}


void CameraMgr::SetProjectionRatio(float ratio) {
    m_projectionRatio = ratio;

    for (auto camera : m_objects) {

        if (camera == nullptr) continue;

        camera->SetProjectionMatrix();

    }
}


CameraComponent* CameraMgr::GetCurrentCamera() const {
    if (m_currentCamera == nullptr && m_size > 0)
        return m_objects.front();

    return m_currentCamera;
}


void CameraMgr::ChangeCurrentCamera(CameraComponent* camera) {

    m_currentCamera = camera;

}
