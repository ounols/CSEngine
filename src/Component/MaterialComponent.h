#pragma once

#include "../Util/Render/SCubeTexture.h"
#include "SComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/SMaterial.h"

namespace CSE {

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

        void AttachMaterials(std::string id);
        void AttachMaterials(SMaterial* material);

        void CreateMaterialInstance();

        void SetInt(std::string element_name, int value);

        void SetAlbedo(vec3 albedo);

        void SetAlbedoTexture(STexture* albedo);

        void SetMetallic(float metallic);

        void SetMetallicTexture(STexture* metallic);

        void SetRoughness(float roughness);

        void SetRoughnessTexture(STexture* roughness);

        void SetAo(float ao);

        void SetAoTexture(STexture* ao);

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

    private:
        void SetShader(const GLProgramHandle* handle) const;

        bool AttachTexture(STexture* texture, int tex_id, int mtrl_id, unsigned short layout) const;

    private:
        //Materials
        STexture* m_albedoTexture = nullptr;
        STexture* m_metallicTexture = nullptr;
        STexture* m_roughnessTexture = nullptr;
        STexture* m_aoTexture = nullptr;

        vec3 m_albedoMaterial = vec3{ 0.5f, 0.5f, 0.5f };
        float m_metallicMaterial = 0.7f;
        float m_roughnessMaterial = 0.4f;
        float m_aoMaterial = 1.0f;

        SCubeTexture* m_irradianceTexture = nullptr;
        SCubeTexture* m_prefilterTexture = nullptr;
        vec3 m_irradianceMaterial = vec3{ 0.03f, 0.03f, 0.03f };
        bool m_isPrefilter = false;

		SMaterial* m_material = nullptr;
		SMaterial* m_materialInstance = nullptr;
    };

}
