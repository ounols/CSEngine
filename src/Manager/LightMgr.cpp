#include "LightMgr.h"
#include "../OGLDef.h"

enum LIGHTMODE { None, Amb, Dif, Spec, AMbDif = 12, AmbSpec = 13, DifSpec = 23, AmbDifSpec = 123};

IMPLEMENT_SINGLETON(LightMgr);

LightMgr::LightMgr() {
	
}


LightMgr::~LightMgr() {
	
}


void LightMgr::AttachLightToShader(const GLProgramHandle* handle) const {

	if (handle == nullptr) return;

	for(const auto& light : m_objects) {
		
		if (light == nullptr) continue;
		if (!light->GetIsEnable()) continue;

		LightComponent::LIGHT type = light->m_type;
		SLight* lightObject = light->GetLight();

		//LightMode
		SetLightMode(handle, light);

		switch (type) {
			
		case LightComponent::DIRECTIONAL:
			AttachDirectionalLight(handle, lightObject);
			break;
		case LightComponent::POINT:
			AttachPositionalLight(handle, lightObject);
			break;
		case LightComponent::SPOT: break;
		default: break;

		}

		
	}

}


void LightMgr::AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light) const {


	//¹æÇâ
	glUniform4fv(handle->Uniforms.LightPosition, 1, light->direction.Pointer());
	glUniform1i(handle->Uniforms.IsDirectional, 1);

	//°¨¼è¹æÁ¤½Ä
	glUniform1i(handle->Uniforms.IsAttenuation, 0);

	//»ö±¤
	glUniform4fv(handle->Uniforms.DiffuseLight, 1, light->diffuseColor.Pointer());
	glUniform4fv(handle->Uniforms.AmbientLight, 1, light->ambientColor.Pointer());
	glUniform4fv(handle->Uniforms.SpecularLight, 1, light->specularColor.Pointer());



}


void LightMgr::AttachPositionalLight(const GLProgramHandle* handle, const SLight* light) const {

	vec4 position(light->position->x, light->position->y, light->position->z, 1);
	//À§Ä¡
	glUniform4fv(handle->Uniforms.LightPosition, 1, position.Pointer());
	glUniform1i(handle->Uniforms.IsDirectional, 0);

	//°¨¼è¹æÁ¤½Ä
	glUniform1i(handle->Uniforms.IsAttenuation, 1);
	glUniform3fv(handle->Uniforms.AttenuationFactor, 1, light->att.Pointer());
	glUniform1f(handle->Uniforms.LightRadius, light->radius);

	//»ö±¤
	glUniform4fv(handle->Uniforms.DiffuseLight, 1, light->diffuseColor.Pointer());
	glUniform4fv(handle->Uniforms.AmbientLight, 1, light->ambientColor.Pointer());
	glUniform4fv(handle->Uniforms.SpecularLight, 1, light->specularColor.Pointer());

}


void LightMgr::SetLightMode(const GLProgramHandle* handle, const LightComponent* light) {

	GLuint mode = 0;

	if(!light->DisableAmbient) {
		mode = Amb;
	}

	if(!light->DisableDiffuse) {
		mode *= 10;
		mode += Dif;
	}

	if(!light->DisableSpecular) {
		mode *= 10;
		mode += Spec;
	}

	glUniform1i(handle->Uniforms.LightMode, mode);

}
