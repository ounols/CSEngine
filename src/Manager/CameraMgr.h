#pragma once

#include "Base/SContainerList.h"
#include "Base/CoreBase.h"
#include "../Component/CameraComponent.h"

namespace CSE {

    class GLProgramHandle;

    class CameraMgr : public SContainerList<CameraComponent*>, public CoreBase {
    public:
        explicit CameraMgr();
        ~CameraMgr() override;

        void Init() override;

        void DeleteCameraComponent(CameraComponent* object);


        const float* GetProjectionRatio() const;

        void SetProjectionRatio(float ratio);

        CameraComponent* GetCurrentCamera() const;

        void ChangeCurrentCamera(CameraComponent* camera);

        GLProgramHandle* GetSkyboxProgram() const;

    private:
        CameraComponent* m_currentCamera = nullptr;
        float m_projectionRatio = 1.0f;

        // Skybox Raw Material
        GLProgramHandle* m_skyboxProgram = nullptr;
    };
}