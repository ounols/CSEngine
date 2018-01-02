#pragma once
#include "SComponent.h"
#include "../Util/Vector.h"
#include "DrawableStaticMeshComponent.h"

class CameraComponent : public SComponent {
public:
	CameraComponent();
	~CameraComponent();


	void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

	mat4 GetMatrix() const {
		return m_cameraMatrix;
	}

	void SetTarget(vec3 target);
	void SetUp(vec3 up);

private:
	void SetCameraMatrix();

private:
	vec3* m_eye;
	vec3 m_target;
	vec3 m_up;
	SGameObject* m_targetObject;

	mat4 m_cameraMatrix;
	vec3 m_resultTarget;

};

