#pragma once

#include "../Util/Render/SCubeTexture.h"
#include "SComponent.h"
#include "../Util/GLProgramHandle.h"

class MaterialComponent : public SComponent {
public:
    MaterialComponent();

    ~MaterialComponent();


    void Exterminate() override;

    void Init() override;

    void Tick(float elapsedTime) override;

    SComponent* Clone(SGameObject* object) override;

    void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                       std::map<SComponent*, SComponent*> lists_comp) override;

    void AttachMaterials(const GLProgramHandle* handle) const;

    void SetMaterialAmbient(vec3 color);

    void SetMaterialSpecular(vec3 color);

    void SetShininess(float value);

    void SetDiffuseMaterial(vec4 diffuse_material);

    void SetTexture(STexture* texture);


    vec4 GetDiffuseMaterial() const;

    vec3 GetAmbientMaterial() const;

    vec3 GetSpecularMaterial() const;

    float GetShininess() const;

    STexture* GetTexture() const;

private:
    //NonPBR=================================================
    vec3 m_ambientMaterial = vec3{ 0.5f, 0.5f, 0.5f };
    vec3 m_specularMaterial = vec3{ 1, 1, 1 };
    vec4 m_diffuseMaterial = vec4{ 0.75f, 0.75f, 0.75f, 1 };
    float m_shininess = 128;
    //=======================================================
    vec3 m_albedoMaterial = vec3{ 0.5f, 0.5f, 0.5f };
    STexture* m_albedoTexture = nullptr;
    float m_metallicMaterial = 0;
    STexture* m_metallicTexture = nullptr;
    float m_roughnessMaterial = 0;
    STexture* m_roughnessTexture = nullptr;
    float m_aoMaterial = 0;
    STexture* m_aoTexture = nullptr;

    SCubeTexture* m_irradianceTexture = nullptr;

    std::map<std::string, std::string> m_attribBindList;
    std::map<std::string, std::string> m_uniformBindList;

    bool m_isPBR = true;

};

