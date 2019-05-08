#include "../Manager/TextureContainer.h"
#include "MaterialComponent.h"


COMPONENT_CONSTRUCTOR(MaterialComponent) {

}


MaterialComponent::~MaterialComponent() {}


void MaterialComponent::Exterminate() {
}


void MaterialComponent::Init() {
    if(m_irradianceTexture == nullptr) {
        m_irradianceTexture = ResMgr::getInstance()->GetObject<TextureContainer, SCubeTexture>(3);
    }
}


void MaterialComponent::Tick(float elapsedTime) {
}


void MaterialComponent::AttachMaterials(const GLProgramHandle* handle) const {

//    if(!m_isPBR) {
//        glUniform3fv(handle->Uniforms.AmbientMaterial, 1, m_ambientMaterial.Pointer());
//        glUniform3fv(handle->Uniforms.SpecularMaterial, 1, m_specularMaterial.Pointer());
//        handle->SetAttribVec4("")
//        glUniform1f(handle->Uniforms.Shininess, m_shininess);
//
//    }




	if(m_albedoTexture != nullptr) {
        m_albedoTexture->Bind(handle->UniformLocation("TEX2D_ALBEDO"), 0);
	}

    if(m_irradianceTexture != nullptr) {
        m_irradianceTexture->Bind(0, 1);
    }

}


void MaterialComponent::SetMaterialAmbient(vec3 color) {
	m_ambientMaterial = color;
}


void MaterialComponent::SetMaterialSpecular(vec3 color) {
	m_specularMaterial = color;
}


void MaterialComponent::SetShininess(float value) {
	m_shininess = value;
}

void MaterialComponent::SetDiffuseMaterial(vec4 diffuse_material) {
	m_diffuseMaterial = diffuse_material;
}

void MaterialComponent::SetTexture(STexture* texture) {
	m_albedoTexture = texture;
}

vec4 MaterialComponent::GetDiffuseMaterial() const {
	return m_diffuseMaterial;
}




vec3 MaterialComponent::GetAmbientMaterial() const {
	return m_ambientMaterial;
}


vec3 MaterialComponent::GetSpecularMaterial() const {
	return m_specularMaterial;
}


float MaterialComponent::GetShininess() const {
	return m_shininess;
}

STexture* MaterialComponent::GetTexture() const {
	return m_albedoTexture;
}

SComponent* MaterialComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(MaterialComponent, clone);

    clone->m_ambientMaterial = m_ambientMaterial;
    clone->m_specularMaterial = m_specularMaterial;
    clone->m_diffuseMaterial = m_diffuseMaterial;
    clone->m_shininess = m_shininess;
    clone->m_albedoTexture = m_albedoTexture;

    return clone;
}

void MaterialComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                      std::map<SComponent*, SComponent*> lists_comp) {
    return;
}