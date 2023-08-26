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

    int index = 0;
    int shadow_index = 0;
    for (const auto& light : m_objects) {
        if (light == nullptr || !light->GetIsEnable()) continue;

        const auto& lightObject = light->GetLight();
        const auto& position = (light->m_type == LightComponent::DIRECTIONAL)
                               ? lightObject->direction
                               : vec4(*lightObject->position, 1.0f);

        glUniform4f(handle->Uniforms.LightPosition + index,
                    position.x, position.y, position.z, (light->m_type == LightComponent::DIRECTIONAL) ? 0.0f : 1.0f);
        glUniform3f(handle->Uniforms.LightColor + index, lightObject->color.x, lightObject->color.y, lightObject->color.z);
        glUniform1i(handle->Uniforms.LightType + index, light->m_type);
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

int LightMgr::AttachLightMapToShader(const GLProgramHandle* handle, int textureLayout) const {
    return m_environmentMgr->BindPBREnvironmentMap(handle, textureLayout);
}

void LightMgr::Init() {
    // Setting PBS Environment
    m_environmentMgr = new SEnvironmentMgr();
    m_environmentMgr->RenderPBREnvironment();
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