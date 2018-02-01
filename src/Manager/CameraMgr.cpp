#include "CameraMgr.h"

IMPLEMENT_SINGLETON(CameraMgr);

CameraMgr::CameraMgr() {
}


CameraMgr::~CameraMgr() {}


void CameraMgr::Init() {
}


void CameraMgr::Tick() {
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


CameraComponent* CameraMgr::GetCurrentCamera() const {
	if (m_currentCamera == nullptr && m_size > 0)
		return m_objects.at(0);

	return m_currentCamera;
}


void CameraMgr::ChangeCurrentCamera(CameraComponent* camera) {

	m_currentCamera = camera;

}
