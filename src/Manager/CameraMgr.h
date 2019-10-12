#pragma once
#include "../Component/CameraComponent.h"


namespace CSE {

    class CameraMgr : public SContainer<CameraComponent*> {
    public:
    DECLARE_SINGLETONE(CameraMgr);

        ~CameraMgr();

        void Init();

        void Tick();

        void DeleteCameraComponent(CameraComponent* object);


        const float* GetProjectionRatio() const;

        void SetProjectionRatio(float ratio);

        CameraComponent* GetCurrentCamera() const;

        void ChangeCurrentCamera(CameraComponent* camera);

    private:
        CameraComponent* m_currentCamera = nullptr;
        float m_projectionRatio;

    };
}