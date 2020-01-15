#include "LightMgr.h"
#include "../OGLDef.h"

using namespace CSE;

enum LIGHTMODE {
    None, Amb, Dif, Spec, AMbDif = 12, AmbSpec = 13, DifSpec = 23, AmbDifSpec = 123
};

IMPLEMENT_SINGLETON(LightMgr);

LightMgr::LightMgr() {

}


LightMgr::~LightMgr() {

}


void LightMgr::AttachLightToShader(const GLProgramHandle* handle) const {

    if (handle == nullptr) return;

    //¶óÀÌÆ® °¹¼ö¸¦ ½¦ÀÌ´õ¿¡ ³Ö¾îÁÜ (ÇÊ¿ä¾øµû)
//	glUniform1i(handle->Uniforms.LightsSize, m_objects.size());

    //std::vector<float> lightPosition;
    //std::vector<int> lightType;
    //std::vector<float> lightRadius;
    //std::vector<float> lightColor;

    int i = 0;
    for (const auto& light : m_objects) {

        if (light == nullptr) continue;
        if (!light->GetIsEnable()) continue;

        LightComponent::LIGHT type = light->m_type;
        SLight* lightObject = light->GetLight();

		float lightPosition[4] = { 0.f };

        //LightMode
        //SetLightMode(handle, light, i);

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


		glUniform4f(handle->Uniforms.LightPosition + i, lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);
		glUniform3f(handle->Uniforms.LightColor + i, lightObject->color.x, lightObject->color.y, lightObject->color.z);
		glUniform1i(handle->Uniforms.LightType + i, type);
		glUniform1f(handle->Uniforms.LightRadius + i, lightObject->radius);

        i++;
    }

    if (i <= 0) return;

    glUniform1i(handle->Uniforms.LightSize, m_objects.size());

}


//void LightMgr::AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light, int index) const {
//
//
//	//¹æÇâ
////	glUniform4fv(handle->Uniforms.LightPosition[index], 1, light->direction.Pointer());
//	glUniform1i(handle->Uniforms.IsDirectional[index], 1);
//
//	//°¨¼è¹æÁ¤½Ä
//	glUniform1i(handle->Uniforms.IsAttenuation[index], 0);
//
//	//»ö±¤
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
//	//À§Ä¡
//	glUniform4fv(handle->Uniforms.LightPosition[index], 1, position.Pointer());
//	glUniform1i(handle->Uniforms.IsDirectional[index], 0);
//
//	//°¨¼è¹æÁ¤½Ä
//	glUniform1i(handle->Uniforms.IsAttenuation[index], 1);
//	glUniform3fv(handle->Uniforms.AttenuationFactor[index], 1, light->att.Pointer());
//	glUniform1f(handle->Uniforms.LightRadius[index], light->radius);
//
//	//»ö±¤
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
