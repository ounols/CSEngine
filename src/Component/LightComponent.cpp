#include "LightComponent.h"
#include "CameraComponent.h"
#include "../Manager/LightMgr.h"
#include "TransformComponent.h"
#include "../Manager/EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/GLProgramHandle.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(LightComponent) {

    auto lightMgr = CORE->GetCore(LightMgr);
    lightMgr->Register(this);

    m_light = new SLight();

    SetLightType(DIRECTIONAL);
}


LightComponent::~LightComponent() = default;


void LightComponent::Exterminate() {

    CORE->GetCore(LightMgr)->Remove(this);
    SAFE_DELETE(m_light);
    auto resMgr = CORE->GetCore(ResMgr);
    if(m_frameBuffer != nullptr) resMgr->Remove(m_frameBuffer);
}


void LightComponent::Init() {
    if(m_disableShadow) return;
    SetDepthMap();
}


void LightComponent::Tick(float elapsedTime) {

    auto eye = static_cast<TransformComponent*>(gameObject->GetTransform())->GetPosition();
    auto target = *eye + vec3(-m_light->direction.x, -m_light->direction.y, -m_light->direction.z);
    m_lightViewMatrix = mat4::LookAt(*eye, target, vec3{ 0, 1, 0 });

    if (m_isSunRising && m_type == DIRECTIONAL) {
        float value = m_light->direction.y;
        float bright = (value < 0.2) ? (value - 0.2f) * 2 + 1 : 1;
        if (bright < 0) bright = 0;
        if (value < 0) value = 0;

        float color0 = (0.4f * (1 - value) + 0.4f) * bright * 5;
        float color1 = (0.3f * value + 0.3f) * bright * 5;

//		float color2 = (value_str * 0.07f + 0.03f) * 10;

        m_light->color = vec3{ color0, color1, color1 };
    }
}


void LightComponent::SetLightType(LIGHT type) {

    m_type = type;


    if (m_type == POINT || m_type == SPOT) {
        SetLightPosition();
    }
    else {
        m_lightProjectionMatrix = mat4::Ortho(-3.f, 3.f, -3.f, 3.f, m_near, m_far);
    }

}


void LightComponent::SetDirection(const vec4& direction) const {

    switch (m_type) {

        case DIRECTIONAL:
        case SPOT:

            m_light->direction = direction;
            return;

        default:
            return;
    }

}


void LightComponent::SetColor(const vec3& color) const {
    m_light->color = color;

}

void LightComponent::SetLightRadius(float radius) const {
    m_light->radius = radius;
}


void LightComponent::SetAttenuationFactor(const vec3& att) const {
    m_light->att = att;
}


void LightComponent::SetAttenuationFactor(float Kc, float Kl, float Kq) const {
    SetAttenuationFactor(vec3{ Kc, Kl, Kq });
}

void LightComponent::SetLightPosition() const {
    m_light->position = static_cast<TransformComponent*>(gameObject->GetTransform())->GetPosition();
}

void LightComponent::SetDepthMap() {
    if(m_frameBuffer != nullptr) return;

    auto lightMgr = CORE->GetCore(LightMgr);
    m_frameBuffer = new SFrameBuffer();
    // 1. Generate a framebuffer.
    // 큐브 텍스쳐도 호환되도록 수정이 필요함 (for point shadow)
    m_frameBuffer->GenerateFramebuffer(SFrameBuffer::PLANE);
    // 2. Generate a depth texture.
    m_depthTexture = m_frameBuffer->GenerateTexturebuffer(SFrameBuffer::DEPTH,
                                                          lightMgr->SHADOW_WIDTH, lightMgr->SHADOW_HEIGHT,
                                                          GL_DEPTH_COMPONENT);
    m_depthTexture->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_depthTexture->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#ifdef ANDROID
    m_depthTexture->SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_depthTexture->SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    m_depthTexture->SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    m_depthTexture->SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    m_depthTexture->SetParameterfv(GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
    // 3. Rasterize the framebuffer.
    m_frameBuffer->RasterizeFramebuffer();
}

LightComponent::LIGHT LightComponent::GetType() const {
    return m_type;
}

vec4 LightComponent::GetDirection(const vec4& direction) const {
    return m_light->direction;
}

vec3 LightComponent::GetColor() const {
    return m_light->color;
}

void LightComponent::SetSunrising(bool active) {
    m_isSunRising = active;

}

SComponent* LightComponent::Clone(SGameObject* object) {
	INIT_COMPONENT_CLONE(LightComponent, clone);

	clone->m_type = m_type;
	clone->DisableAmbient = DisableAmbient;
	clone->DisableDiffuse = DisableDiffuse;
	clone->DisableSpecular = DisableSpecular;
	clone->m_disableShadow = m_disableShadow;

	clone->m_near = m_near;
	clone->m_far = m_far;

	return clone;
}

void LightComponent::SetValue(std::string name_str, Arguments value) {
}

std::string LightComponent::PrintValue() const {
	PRINT_START("component");

	PRINT_VALUE(m_type, static_cast<int>(m_type));
	//PRINT_VALUE(DisableAmbient, m_startTime);
	//PRINT_VALUE(m_currentAnimation, ConvertSpaceStr(m_currentAnimation->GetID()));
	//PRINT_VALUE(m_entity, ConvertSpaceStr(m_entity->GetGameObject()->GetID(m_entity)));


	PRINT_END("component");
}

const mat4& LightComponent::GetLightProjectionMatrix() const {
    return m_lightProjectionMatrix;
}

const mat4& LightComponent::GetLightViewMatrix() const {
    return m_lightViewMatrix;
}

void LightComponent::BindShadow(const GLProgramHandle& handle, int handleIndex, int index) const {
    if(m_frameBuffer == nullptr || m_disableShadow) return;

    m_depthTexture->Bind(handle.Uniforms.LightShadowMap + index, index);
    auto matrix = m_lightViewMatrix * m_lightProjectionMatrix;
    glUniformMatrix4fv(handle.Uniforms.LightMatrix + handleIndex, 1, 0, matrix.Pointer());
}

CameraMatrixStruct LightComponent::GetCameraMatrixStruct() const {
    const auto& position = gameObject->GetTransform();
    return { m_lightViewMatrix, m_lightProjectionMatrix, position->m_position };
}

SFrameBuffer* LightComponent::GetFrameBuffer() const {
    return m_frameBuffer;
}
