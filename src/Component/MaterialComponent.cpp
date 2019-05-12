#include "../Manager/TextureContainer.h"
#include "MaterialComponent.h"

const vec3 VEC3_NONE = vec3{ -1, -1, -1 };

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

    if(m_shaderId.fAlbedo == HANDLE_NULL) {
       SetShaderIds(handle);
    }

    //albedo
    if(m_albedoTexture != nullptr) {
        m_albedoTexture->Bind(m_shaderId.texAlbedo, 0);
        glUniform3fv(m_shaderId.fAlbedo, 1, VEC3_NONE.Pointer());
    }
	else {
        glUniform3fv(m_shaderId.fAlbedo, 1, m_albedoMaterial.Pointer());
	}

	//metallic
    if(!AttachTexture(m_metallicTexture, m_shaderId.texMetallic, m_shaderId.fMetallic)) {
        glUniform1f(m_shaderId.fMetallic, m_metallicMaterial);
    }

    //roughness
    if(!AttachTexture(m_roughnessTexture, m_shaderId.texRoughness, m_shaderId.fRoughness)) {
        glUniform1f(m_shaderId.fRoughness, m_roughnessMaterial);
    }

    //AO
    if(!AttachTexture(m_aoTexture, m_shaderId.texAo, m_shaderId.fAo)) {
        glUniform1f(m_shaderId.fAo, m_aoMaterial);
    }

    if(m_irradianceTexture != nullptr) {
        m_irradianceTexture->Bind(0, 1);
    }

}


SComponent* MaterialComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(MaterialComponent, clone);



    return clone;
}

void MaterialComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                      std::map<SComponent*, SComponent*> lists_comp) {
    return;
}

void MaterialComponent::SetShaderIds(const GLProgramHandle* handle) const {
    m_shaderId.fAlbedo = handle->UniformLocation("FLOAT_ALBEDO");
    m_shaderId.fMetallic = handle->UniformLocation("FLOAT_METALLIC");
    m_shaderId.fRoughness = handle->UniformLocation("FLOAT_ROUGHNESS");
    m_shaderId.fAo = handle->UniformLocation("FLOAT_AO");

    m_shaderId.texAlbedo = handle->UniformLocation("TEX2D_ALBEDO");
    m_shaderId.texMetallic = handle->UniformLocation("TEX2D_METALLIC");
    m_shaderId.texRoughness = handle->UniformLocation("TEX2D_ROUGHNESS");
    m_shaderId.texAo = handle->UniformLocation("TEX2D_AO");
}

bool MaterialComponent::AttachTexture(STexture* texture, int tex_id, int mtrl_id) const {
    if(texture != nullptr) {
        texture->Bind(tex_id, 0);
        glUniform1f(mtrl_id, -1);
        return true;
    }
    return false;
}

void MaterialComponent::SetAlbedo(vec3 albedo) {
    m_albedoMaterial = vec3(albedo);
}

void MaterialComponent::SetAlbedoTexture(STexture* albedo) {
    m_albedoTexture = albedo;
    m_albedoMaterial = VEC3_NONE;
}

void MaterialComponent::SetMetallic(float metallic) {
    m_metallicMaterial = metallic;
}

void MaterialComponent::SetMetallicTexture(STexture* metallic) {
    m_metallicTexture = metallic;
    m_metallicMaterial = -1;
}

void MaterialComponent::SetRoughness(float roughness) {
    m_roughnessMaterial = roughness;
}

void MaterialComponent::SetRoughnessTexture(STexture* roughness) {
    m_roughnessTexture = roughness;
    m_roughnessMaterial = -1;
}

void MaterialComponent::SetAo(float ao) {
    m_aoMaterial = ao;
}

void MaterialComponent::SetAoTexture(STexture* ao) {
    m_aoTexture = ao;
    m_aoMaterial = -1;
}
