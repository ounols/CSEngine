#include "MaterialComponent.h"


COMPONENT_CONSTRUCTOR(MaterialComponent) {}


MaterialComponent::~MaterialComponent() {}


void MaterialComponent::Exterminate() {
}


void MaterialComponent::Init() {
}


void MaterialComponent::Tick(float elapsedTime) {
}


void MaterialComponent::AttachMaterials(const GLProgramHandle* handle) const {

	glUniform3fv(handle->Uniforms.AmbientMaterial, 1, m_ambientMaterial.Pointer());
	glUniform3fv(handle->Uniforms.SpecularMaterial, 1, m_specularMaterial.Pointer());
	glVertexAttrib4fv(handle->Attributes.DiffuseMaterial, m_diffuseMaterial.Pointer());
	glUniform1f(handle->Uniforms.Shininess, m_shininess);

	if(m_texture != nullptr) {
		m_texture->Bind(handle);
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
	m_texture = texture;
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
	return m_texture;
}

SComponent* MaterialComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(MaterialComponent, clone);

    clone->m_ambientMaterial = m_ambientMaterial;
    clone->m_specularMaterial = m_specularMaterial;
    clone->m_diffuseMaterial = m_diffuseMaterial;
    clone->m_shininess = m_shininess;
    clone->m_texture = m_texture;

    return clone;
}

void MaterialComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                      std::map<SComponent*, SComponent*> lists_comp) {
    return;
}