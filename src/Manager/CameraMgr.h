#pragma once
#include "../Component/CameraComponent.h"


class CameraMgr : public SContainer<CameraComponent*> {
public:
	DECLARE_SINGLETONE(CameraMgr);
	~CameraMgr();

	void Init();
	void Tick();
	void DeleteCameraComponent(CameraComponent* object);

	CameraComponent* GetCurrentCamera() const;
	void ChangeCurrentCamera(CameraComponent* camera);

private:
	CameraComponent* m_currentCamera = nullptr;

};

