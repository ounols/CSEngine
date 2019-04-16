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

	//����Ʈ ������ ���̴��� �־���
	glUniform1i(handle->Uniforms.LightsSize, m_objects.size());

	int i = 0;
	for(const auto& light : m_objects) {
		
		if (light == nullptr) continue;
		if (!light->GetIsEnable()) continue;

		LightComponent::LIGHT type = light->m_type;
		SLight* lightObject = light->GetLight();

		//LightMode
		SetLightMode(handle, light, i);

		switch (type) {
			
		case LightComponent::DIRECTIONAL:
			AttachDirectionalLight(handle, lightObject, i);
			break;
		case LightComponent::POINT:
			AttachPositionalLight(handle, lightObject, i);
			break;
		case LightComponent::SPOT: break;
		default: break;

		}

		i++;
	}

}


void LightMgr::AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light, int index) const {


	//����
	glUniform4fv(handle->Uniforms.LightPosition[index], 1, light->direction.Pointer());
	glUniform1i(handle->Uniforms.IsDirectional[index], 1);

	//���������
	glUniform1i(handle->Uniforms.IsAttenuation[index], 0);

	//����
	glUniform4fv(handle->Uniforms.DiffuseLight[index], 1, light->diffuseColor.Pointer());
	glUniform4fv(handle->Uniforms.AmbientLight[index], 1, light->ambientColor.Pointer());
	glUniform4fv(handle->Uniforms.SpecularLight[index], 1, light->specularColor.Pointer());



}


void LightMgr::AttachPositionalLight(const GLProgramHandle* handle, const SLight* light, int index) const {

	vec4 position(light->position->x, light->position->y, light->position->z, 1);
	//��ġ
	glUniform4fv(handle->Uniforms.LightPosition[index], 1, position.Pointer());
	glUniform1i(handle->Uniforms.IsDirectional[index], 0);

	//���������
	glUniform1i(handle->Uniforms.IsAttenuation[index], 1);
	glUniform3fv(handle->Uniforms.AttenuationFactor[index], 1, light->att.Pointer());
	glUniform1f(handle->Uniforms.LightRadius[index], light->radius);

	//����
	glUniform4fv(handle->Uniforms.DiffuseLight[index], 1, light->diffuseColor.Pointer());
	glUniform4fv(handle->Uniforms.AmbientLight[index], 1, light->ambientColor.Pointer());
	glUniform4fv(handle->Uniforms.SpecularLight[index], 1, light->specularColor.Pointer());

}


void LightMgr::SetLightMode(const GLProgramHandle* handle, const LightComponent* light, int index) {

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

	glUniform1i(handle->Uniforms.LightMode[index], mode);

}
