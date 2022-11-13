#pragma once

#include "SComponent.h"
#include "../Util/Vector.h"
#include "../Util/Render/CameraBase.h"

namespace CSE {
    struct CameraMatrixStruct {
        mat4 camera;
        mat4 projection;
        vec3 cameraPosition;
        bool isCube;

        CameraMatrixStruct(const mat4& camera, const mat4& projection, const vec3& cameraPosition, bool isCube = false)
                : camera(camera), projection(projection), cameraPosition(cameraPosition), isCube(isCube) {}
    };

    static mat4 CubeCameraMatrix[] = {
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
            mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f))
    };

    class SFrameBuffer;
    class STexture;

    class CameraComponent : public SComponent, public CameraBase {
    public:
        enum CAMERATYPE {
            PERSPECTIVE = 1, ORTHO = 2, CUBE = 3
        };
    public:
        CameraComponent();

        ~CameraComponent() override;

        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        SComponent* Clone(SGameObject* object) override;

        void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                           std::map<SComponent*, SComponent*> lists_comp) override;

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

        mat4 GetCameraMatrix() const;

        vec3 GetCameraPosition() const;

        mat4 GetProjectionMatrix() const {
            if (!m_isProjectionInited) {
                SetProjectionMatrix();
            }
            return m_projectionMatrix;
        }

        CameraMatrixStruct GetCameraMatrixStruct() const override;

        void SetTargetVector(const vec3& target);

        void SetTarget(SGameObject* gameObject);

        void SetUp(const vec3& up);

        void SetCameraType(CAMERATYPE type);

        void SetPerspectiveFov(float fov);

        void SetZDepthRange(float near, float far);

        void SetPerspective(float fov, float near, float far);

        void SetOrtho(float left, float right, float top, float bottom);

        void SetCubeCamera();

        void SetProjectionMatrix() const;

        SFrameBuffer* GetFrameBuffer() const override;

        void SetFrameBuffer(SFrameBuffer* frameBuffer);

        BackgroundType GetBackgroundType() override;

        void RenderBackground() const override;

        void SetBackgroundSkybox(STexture* skyboxTexture = nullptr);

        void SetBackgroundColor(vec3&& color);

        void SetBackgroundType(BackgroundType type);

    private:
        struct BackgroundMapStruct {
            STexture* map = nullptr;
            unsigned short mapId = 0;
            unsigned short viewId = 0;
            unsigned short projectionId = 0;
        };

    private:
        void SetCameraMatrix();

    private:
        vec3* m_eye;
        vec3 m_target = vec3(0, 0, -1);
        vec3 m_up = vec3(0, 1, 0);
        SGameObject* m_targetObject;

        mutable mat4 m_cameraMatrix;
        mutable mat4 m_projectionMatrix;
        vec3 m_resultTarget;
        SFrameBuffer* m_frameBuffer = nullptr;

        CAMERATYPE m_type = PERSPECTIVE;
        mutable bool m_isProjectionInited = false;

        //perspective
        float m_pFov = 45.f;
        float* m_pRatio;

        //Orthographic
        float m_oLeft = -1.f;
        float m_oRight = 1.f;
        float m_oBottom = -1.f;
        float m_oTop = 1.f;

        float m_Near = 0.1f;
        float m_Far = 100.f;

        //Background
        BackgroundType m_backgroundType = SOLID;
        vec3 m_backgroundColor = vec3(0.4f, 0.4f, 0.4f);
        BackgroundMapStruct* m_backgroundMap = nullptr;
    };
}