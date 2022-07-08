#include "LightMgr.h"
#include "../Component/RenderComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/SEnvironmentMgr.h"

using namespace CSE;

LightMgr::LightMgr() = default;

LightMgr::~LightMgr() {
    SAFE_DELETE(m_environmentMgr);
}


void LightMgr::AttachLightToShader(const GLProgramHandle* handle) const {

    if (handle == nullptr) return;

//	glUniform1i(handle->Uniforms.LightsSize, m_objects.size());

    //std::vector<float> lightPosition;
    //std::vector<int> lightType;
    //std::vector<float> lightRadius;
    //std::vector<float> lightColor;

    int index = 0;
    int shadow_index = 0;
    for (const auto& light : m_objects) {

        if (light == nullptr) continue;
        if (!light->GetIsEnable()) continue;

        LightComponent::LIGHT type = light->m_type;
        SLight* lightObject = light->GetLight();

        float lightPosition[4] = { 0.f };

        //LightMode
        //SetLightMode(handle, light, index);

        switch (type) {

            case LightComponent::DIRECTIONAL:
                lightPosition[0] = lightObject->direction.x;
                lightPosition[1] = lightObject->direction.y;
                lightPosition[2] = lightObject->direction.z;
                lightPosition[3] = lightObject->direction.w;
                break;
            case LightComponent::POINT:
                lightPosition[0] = lightObject->position->x;
                lightPosition[1] = lightObject->position->y;
                lightPosition[2] = lightObject->position->z;
                lightPosition[3] = 1.0f;
                break;
            case LightComponent::SPOT:
                break;
            case LightComponent::NONE:
            default:
                break;
        }
        glUniform4f(handle->Uniforms.LightPosition + index,
                    lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);
        glUniform3f(handle->Uniforms.LightColor + index, lightObject->color.x, lightObject->color.y,
                    lightObject->color.z);
        glUniform1i(handle->Uniforms.LightType + index, type);
        if (handle->Uniforms.LightRadius >= 0)
            glUniform1f(handle->Uniforms.LightRadius + index, lightObject->radius);
        // Shadow
        auto isShadow = light->IsShadow();
        glUniform1i(handle->Uniforms.LightShadowMode + index, isShadow ? 1 : 0);
        light->BindShadow(*handle, index, shadow_index);
        if (isShadow) ++shadow_index;
        ++index;
    }

    if (index <= 0) return;

    glUniform1i(handle->Uniforms.LightSize, (int)m_objects.size());
}

void LightMgr::AttachLightMapToShader(const GLProgramHandle* handle, int textureLayout) const {
    m_environmentMgr->BindPBREnvironmentMap(handle, textureLayout);
}

void LightMgr::Init() {
    // Setting PBS Environment
    m_environmentMgr = new SEnvironmentMgr();
    m_environmentMgr->RenderPBREnvironment();
    // Setting Shadow Environment
    m_environmentMgr->InitShadowEnvironment();
    m_shadowHandle = m_environmentMgr->GetShadowEnvironment();
}

void LightMgr::RegisterShadowObject(SIRender* object) {
    m_shadowObject.push_back(object);
}

void LightMgr::RemoveShadowObject(SIRender* object) {
    m_shadowObject.remove(object);
}

const std::list<SIRender*>& LightMgr::GetShadowObject() const {
    return m_shadowObject;
}

void LightMgr::RefreshShadowCount(int shadowCount) const {
    if(shadowCount < 0) {
        int tempCount = 0;
        for (const auto& light : m_objects) {
            ++tempCount;
        }
        m_shadowCount = tempCount;
        return;
    }
    m_shadowCount = shadowCount;
}

GLProgramHandle* LightMgr::GetShadowHandle() const {
    return m_shadowHandle;
}
