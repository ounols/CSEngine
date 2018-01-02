#include "CameraMgr.h"


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
		MemoryMgr::getInstance()->ReleaseObject(object);
	}
}
