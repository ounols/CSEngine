#pragma once
#include "../Component/CameraComponent.h"


class CameraMgr : public SContainer<CameraComponent*> {
public:
	CameraMgr();
	~CameraMgr();

	void Init();
	void Tick();
	void DeleteCameraComponent(CameraComponent* object);

};

