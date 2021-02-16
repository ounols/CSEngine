#include "MaterialComponent.h"

using namespace CSE;

const vec3 VEC3_NONE = vec3{ -1, -1, -1 };

COMPONENT_CONSTRUCTOR(MaterialComponent) {

}


MaterialComponent::~MaterialComponent() {}


void MaterialComponent::Exterminate() {
    if(m_materialInstance != nullptr) {
        SAFE_DELETE(m_materialInstance);
    }
}


void MaterialComponent::Init() {

//    if (m_irradianceTexture == nullptr) {
//        m_irradianceTexture = ResMgr::getInstance()->GetObject<SCubeTexture>("irradiance.textureCubeMap");
//    }
//    if (m_prefilterTexture == nullptr) {
//        m_prefilterTexture = ResMgr::getInstance()->GetObject<SCubeTexture>("prefilter.textureCubeMap");
//    }
}


void MaterialComponent::Tick(float elapsedTime) {
}


void MaterialComponent::AttachMaterials(std::string id) {
    m_material = SResource::Create<SMaterial>(id);
}

void MaterialComponent::AttachMaterials(SMaterial* material) {

    m_material = material;


//    //albedo
//    if (m_albedoTexture != nullptr) {
//        m_albedoTexture->Bind(m_shaderId.texAlbedo, 0);
//        glUniform3fv(m_shaderId.fAlbedo, 1, VEC3_NONE.Pointer());
//    } else {
//        glUniform1i(m_shaderId.texAlbedo, 0);
//        glUniform3fv(m_shaderId.fAlbedo, 1, m_albedoMaterial.Pointer());
//    }
//
//    //metallic
//    if (!AttachTexture(m_metallicTexture, m_shaderId.texMetallic, m_shaderId.fMetallic, 1)) {
//        glUniform1f(m_shaderId.fMetallic, m_metallicMaterial);
//    }
//
//    //roughness
//    if (!AttachTexture(m_roughnessTexture, m_shaderId.texRoughness, m_shaderId.fRoughness, 2)) {
//        glUniform1f(m_shaderId.fRoughness, m_roughnessMaterial);
//    }
//
//    //AO
//    if (!AttachTexture(m_aoTexture, m_shaderId.texAo, m_shaderId.fAo, 3)) {
//        glUniform1f(m_shaderId.fAo, m_aoMaterial);
//    }
//
//    //irradiance
//    if (m_irradianceTexture != nullptr) {
//        m_irradianceTexture->Bind(m_shaderId.texIrradiance, 4);
//        glUniform3fv(m_shaderId.fIrradiance, 1, VEC3_NONE.Pointer());
//    } else {
//        glUniform1i(m_shaderId.texIrradiance, 4);
//        glUniform3fv(m_shaderId.fIrradiance, 1, m_irradianceMaterial.Pointer());
//    }
//
//    //prefilter
//    if (m_prefilterTexture != nullptr) {
//        m_prefilterTexture->Bind(m_shaderId.texPrefilter, 5);
//        glUniform1i(m_shaderId.bPrefilter, 1);
//    } else {
//        glUniform1i(m_shaderId.texIrradiance, 5);
//        glUniform1i(m_shaderId.bPrefilter, 0);
//    }

}


SComponent* MaterialComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(MaterialComponent, clone);

    clone->m_material = m_material;
    if(clone->m_materialInstance != nullptr) {
        m_materialInstance = new SMaterial(m_material);
    }

    return clone;
}

void MaterialComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                      std::map<SComponent*, SComponent*> lists_comp) {
    return;
}

void MaterialComponent::SetShader(const GLProgramHandle* handle) const {
//	m_shaderId.fAlbedo = handle->UniformLocation("FLOAT_ALBEDO")->id;
//    m_shaderId.fMetallic = handle->UniformLocation("FLOAT_METALLIC")->id;
//    m_shaderId.fRoughness = handle->UniformLocation("FLOAT_ROUGHNESS")->id;
//    m_shaderId.fAo = handle->UniformLocation("FLOAT_AO")->id;
//    m_shaderId.fIrradiance = handle->UniformLocation("FLOAT_IRRADIANCE")->id;
//
//    m_shaderId.texAlbedo = handle->UniformLocation("TEX2D_ALBEDO")->id;
//    m_shaderId.texMetallic = handle->UniformLocation("TEX2D_METALLIC")->id;
//    m_shaderId.texRoughness = handle->UniformLocation("TEX2D_ROUGHNESS")->id;
//    m_shaderId.texAo = handle->UniformLocation("TEX2D_AO")->id;
//    m_shaderId.texIrradiance = handle->UniformLocation("TEXCUBE_IRRADIANCE")->id;
//
//    m_shaderId.isInited = true;
}

bool MaterialComponent::AttachTexture(STexture* texture, int tex_id, int mtrl_id, unsigned short layout) const {
//    if (texture != nullptr) {
//        texture->Bind(tex_id, layout);
//        glUniform1f(mtrl_id, -1);
//        return true;
//    }
//    return false;
    return false;
}

void MaterialComponent::CreateMaterialInstance() {
    SAFE_DELETE(m_materialInstance);
    m_materialInstance = new SMaterial(m_material);
}

void MaterialComponent::SetAlbedo(vec3 albedo) {
    m_albedoMaterial = albedo;
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

void MaterialComponent::SetInt(std::string element_name, int value) {
    if(m_materialInstance == nullptr) CreateMaterialInstance();

    m_materialInstance->SetElements(element_name, reinterpret_cast<void*>(value));
}

void MaterialComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {
    if (name_str == "m_material") {
        AttachMaterials(value[0]);
    }
}

std::string MaterialComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE(m_material, ConvertSpaceStr(m_material->GetID()));
//    PRINT_VALUE(m_material, "File:DefaultPBR.material");

    PRINT_END("component");
}
