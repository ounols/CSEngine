#pragma once

#include "SComponent.h"
#include "../Util/Render/LightHelper.h"
#include "../Util/Render/STexture.h"

namespace CSE {

    class LightComponent : public SComponent {

    public:
        enum LIGHT {
            NONE = 0, DIRECTIONAL = 1, POINT = 2, SPOT = 3
        };
    public:
        LightComponent();

        ~LightComponent();


        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void SetLightType(LIGHT type);

        void SetDirection(vec4 direction) const;

        void SetColor(vec3 color) const;

        LIGHT GetType() const;

        vec4 GetDirection(vec4 direction) const;

        vec3 GetColor() const;

        //for Positional Light and Spot Light
        void SetLightRadius(float radius) const;

        void SetAttenuationFactor(vec3 att) const;

        void SetAttenuationFactor(float Kc, float Kl, float Kq) const;

        void SetSunrising(bool active);

        SLight* GetLight() const {
            return m_light;
        }

		SComponent* Clone(SGameObject* object) override;

		void SetValue(std::string name_str, Arguments value) override;

		std::string PrintValue() const override;

        const mat4& GetLightProjectionMatrix() const;

        const mat4& GetLightViewMatrix() const;

        void BindDepthMap() const;

    private:
        void SetLightPosition() const;
        void SetDepthMap();

    public:
        LIGHT m_type = DIRECTIONAL;
        bool DisableAmbient = false;
        bool DisableDiffuse = false;
        bool DisableSpecular = true;
        bool m_disableShadow = false;

    private:
        SLight* m_light = nullptr;
        bool m_isSunRising = false;
        unsigned int m_depthMapFBO = -1;
        STexture* m_shadowTexture = nullptr;
        mat4 m_lightProjectionMatrix;
        mat4 m_lightViewMatrix;
        float m_near = 0.1f;
        float m_far = 100.f;
    };

}