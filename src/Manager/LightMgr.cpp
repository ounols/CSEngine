#include "LightMgr.h"
#include "../OGLDef.h"
#include "../Component/RenderComponent.h"
#include "../Util/AssetsDef.h"

using namespace CSE;

enum LIGHTMODE {
    None, Amb, Dif, Spec, AMbDif = 12, AmbSpec = 13, DifSpec = 23, AmbDifSpec = 123
};

LightMgr::LightMgr() {

}


LightMgr::~LightMgr() {

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
            default:
                break;
        }
        glUniform4f(handle->Uniforms.LightPosition + index,
                    lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);
        glUniform3f(handle->Uniforms.LightColor + index, lightObject->color.x, lightObject->color.y,
                    lightObject->color.z);
        glUniform1i(handle->Uniforms.LightType + index, type);
        glUniform1f(handle->Uniforms.LightRadius + index, lightObject->radius);
        if (handle->Uniforms.LightRadius >= 0)
            glUniform1f(handle->Uniforms.LightRadius + index, lightObject->radius);
        // Shadow
        glUniform1i(handle->Uniforms.LightShadowMode + index, !light->m_disableShadow);
        light->BindShadow(*handle, index, shadow_index);
        if (!light->m_disableShadow) ++shadow_index;
        ++index;
    }

    if (index <= 0) return;

    glUniform1i(handle->Uniforms.LightSize, m_objects.size());
}

void LightMgr::RenderShadowMap(GLProgramHandle* handle) const {
    if(handle == nullptr) return;
    glViewport(0, 0, (GLsizei) SHADOW_WIDTH, (GLsizei) SHADOW_HEIGHT);

    glUseProgram(handle->Program);

    // Initialize various state.
    glEnableVertexAttribArray(handle->Attributes.Position);
    glEnableVertexAttribArray(handle->Attributes.Weight);
    glEnableVertexAttribArray(handle->Attributes.JointId);

    m_shadowCount = 0;
    for (const auto& light : m_objects) {
        if(light->m_disableShadow) continue;
        const auto camera = light->GetCameraMatrixStruct();

        light->BindDepthBuffer();
        for (const auto& shadowObject : m_shadowObject) {
            if(!shadowObject->isRenderActive) continue;
            const auto& shadow_transform = static_cast<RenderComponent*>(shadowObject)->GetGameObject()->GetTransform();

            if(SHADOW_DISTANCE < vec3::Distance(camera.cameraPosition, shadow_transform->m_position))
                continue;

            shadowObject->SetMatrix(camera, handle);
            shadowObject->Render(handle);
        }
        ++m_shadowCount;
    }
    glDisableVertexAttribArray(handle->Attributes.Position);
    glDisableVertexAttribArray(handle->Attributes.Weight);
    glDisableVertexAttribArray(handle->Attributes.JointId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightMgr::Init() {

}

void LightMgr::RegisterShadowObject(SIRender* object) {
    m_shadowObject.push_back(object);
}

void LightMgr::RemoveShadowObject(SIRender* object) {
    m_shadowObject.remove(object);
}

//void LightMgr::AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light, int index) const {
//
//
//	//����
////	glUniform4fv(handle->Uniforms.LightPosition[index], 1, light->direction.Pointer());
//	glUniform1i(handle->Uniforms.IsDirectional[index], 1);
//
//	//���������
//	glUniform1i(handle->Uniforms.IsAttenuation[index], 0);
//
//	//����
//	glUniform4fv(handle->Uniforms.DiffuseLight[index], 1, light->diffuseColor.Pointer());
//	glUniform4fv(handle->Uniforms.AmbientLight[index], 1, light->ambientColor.Pointer());
//	glUniform4fv(handle->Uniforms.SpecularLight[index], 1, light->specularColor.Pointer());
//
//
//
//}


//void LightMgr::AttachPositionalLight(const GLProgramHandle* handle, const SLight* light, int index) const {
//
//	vec4 position(light->position->x, light->position->y, light->position->z, 1);
//	//��ġ
//	glUniform4fv(handle->Uniforms.LightPosition[index], 1, position.Pointer());
//	glUniform1i(handle->Uniforms.IsDirectional[index], 0);
//
//	//���������
//	glUniform1i(handle->Uniforms.IsAttenuation[index], 1);
//	glUniform3fv(handle->Uniforms.AttenuationFactor[index], 1, light->att.Pointer());
//	glUniform1f(handle->Uniforms.LightRadius[index], light->radius);
//
//	//����
//	glUniform4fv(handle->Uniforms.DiffuseLight[index], 1, light->diffuseColor.Pointer());
//	glUniform4fv(handle->Uniforms.AmbientLight[index], 1, light->ambientColor.Pointer());
//	glUniform4fv(handle->Uniforms.SpecularLight[index], 1, light->specularColor.Pointer());
//
//}


//void LightMgr::SetLightMode(const GLProgramHandle* handle, const LightComponent* light, int index) {
//
//	GLuint mode = 0;
//
//	if(!light->DisableAmbient) {
//		mode = Amb;
//	}
//
//	if(!light->DisableDiffuse) {
//		mode *= 10;
//		mode += Dif;
//	}
//
//	if(!light->DisableSpecular) {
//		mode *= 10;
//		mode += Spec;
//	}
//
//	glUniform1i(handle->Uniforms.LightMode[index], mode);
//
//}
