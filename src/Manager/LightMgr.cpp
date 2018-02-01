#include "LightMgr.h"
#include "../OGLDef.h"

IMPLEMENT_SINGLETON(LightMgr);

LightMgr::LightMgr() {
	
}


LightMgr::~LightMgr() {
	
}


void LightMgr::AttachLightToShader(const GLProgramHandle* handle) const {

	if (handle == nullptr) return;

	for(const auto& light : m_objects) {
		
		if (light == nullptr) continue;
		if (!light->getIsEnable()) continue;

		LightComponent::LIGHT type = light->m_type;
		SLight* lightObject = light->GetLight();

		switch (type) {
			
		case LightComponent::DIRECTIONAL:
			AttachDirectionalLight(handle, lightObject);
			break;
		case LightComponent::POINT: break;
		case LightComponent::SPOT: break;
		default: break;

		}

		
	}

}


void LightMgr::AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light) const {

	vec3 directionLight = vec3{ light->direction.x, light->direction.y, light->direction.z };

	//¹æÇâ
	glUniform4fv(handle->Uniforms.LightPosition, 1, light->direction.Pointer());
	glUniform1i(handle->Uniforms.IsDirectional, (light->is_directional) ? 1 : 0);

	//»ö±¤
	glUniform4fv(handle->Uniforms.DiffuseLight, 1, light->diffuseColor.Pointer());
	glUniform4fv(handle->Uniforms.AmbientLight, 1, light->ambientColor.Pointer());
	glUniform4fv(handle->Uniforms.SpecularLight, 1, light->specularColor.Pointer());



}
