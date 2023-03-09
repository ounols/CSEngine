#pragma once

#include "SComponent.h"
#include "../Util/Render/LightHelper.h"
#include "../Util/Render/STexture.h"
#include "../Util/Render/CameraBase.h"

namespace CSE {

    class GLProgramHandle;
    struct CameraMatrixStruct;

    class LightComponent : public SComponent, public CameraBase {

    public:
        enum LIGHT {
            NONE = 0, DIRECTIONAL = 1, POINT = 2, SPOT = 3
        };
    public:
        COMPONENT_DEFINE_CONSTRUCTOR(LightComponent);

        ~LightComponent() override;


        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void SetLightType(LIGHT type);

        void SetDirection(const vec4& direction) const;

        void SetColor(const vec3& color) const;

        LIGHT GetType() const;

        vec4 GetDirection(const vec4& direction) const;

        vec3 GetColor() const;

        //for Positional Light and SpotLight
        void SetLightRadius(float radius) const;

        void SetAttenuationFactor(const vec3& att) const;

        void SetAttenuationFactor(float Kc, float Kl, float Kq) const;

        void SetSunrising(bool active);

        SLight* GetLight() const {
            return m_light;
        }

        void SetShadow(bool isActive) {
            m_disableShadow = !isActive;
        }

		SComponent* Clone(SGameObject* object) override;

		void SetValue(std::string name_str, Arguments value) override;

		std::string PrintValue() const override;

        const mat4& GetLightProjectionMatrix() const;

        const mat4& GetLightViewMatrix() const;

        CameraMatrixStruct GetCameraMatrixStruct() const override;

        SFrameBuffer* GetFrameBuffer() const override;

        bool IsShadow() const;

        void BindShadow(const GLProgramHandle& handle, int handleIndex, int index) const;

        void RenderBackground() const override {};

    private:
        void SetLightPosition() const;
        void SetDepthMap();

    public:
        LIGHT m_type = DIRECTIONAL;

    private:
        SLight* m_light = nullptr;
        bool m_isSunRising = false;
        bool m_disableShadow = false;
        SFrameBuffer* m_frameBuffer = nullptr;
        STexture* m_depthTexture = nullptr;
        mat4 m_lightProjectionMatrix;
        mat4 m_lightViewMatrix;
        float m_near = -10.f;
        float m_far = 10.f;
    };

}