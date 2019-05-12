#pragma once

#include "../Util/Render/SCubeTexture.h"
#include "SComponent.h"
#include "../Util/GLProgramHandle.h"

class MaterialComponent : public SComponent {
private:
    struct ShaderID {
        int texAlbedo = -1;
        int texMetallic = -1;
        int texRoughness = -1;
        int texAo = -1;

        int fAlbedo = -1;
        int fMetallic = -1;
        int fRoughness = -1;
        int fAo = -1;
    };
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


    void SetAlbedo(vec3 albedo);
    void SetAlbedoTexture(STexture* albedo);

    void SetMetallic(float metallic);
    void SetMetallicTexture(STexture* metallic);

    void SetRoughness(float roughness);
    void SetRoughnessTexture(STexture* roughness);

    void SetAo(float ao);
    void SetAoTexture(STexture* ao);

private:
    void SetShaderIds(const GLProgramHandle* handle) const;
    bool AttachTexture(STexture* texture, int tex_id, int mtrl_id) const;

private:
    //Materials
    STexture* m_albedoTexture = nullptr;
    STexture* m_metallicTexture = nullptr;
    STexture* m_roughnessTexture = nullptr;
    STexture* m_aoTexture = nullptr;

    vec3 m_albedoMaterial = vec3{ 0.5f, 0.5f, 0.5f };
    float m_metallicMaterial = 0;
    float m_roughnessMaterial = 0;
    float m_aoMaterial = 0;

    SCubeTexture* m_irradianceTexture = nullptr;

    //Shader ID
    mutable ShaderID m_shaderId;

};

