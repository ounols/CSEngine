#pragma once

#include "SComponent.h"
#include "../Util/Render/RenderInterfaces.h"
#include "DrawableStaticMeshComponent.h"
#include "DrawableSkinnedMeshComponent.h"
#include "../Util/Render/SMaterial.h"

namespace CSE {

    class RenderComponent : public SComponent, public SIRender {
    public:
        RenderComponent();

        ~RenderComponent();

        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        SComponent* Clone(SGameObject* object) override;


        void
        SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection, const GLProgramHandle* handle = nullptr) override;

        void Render(const GLProgramHandle* handle = nullptr) const override;

        void SetIsEnable(bool is_enable) override;

        SMaterial* GetMaterial() const;

        void SetMaterial(SMaterial* material);

    private:
        void AttachMaterials() const;

        void SetJointMatrix(const GLProgramHandle* handle) const;

    private:
        DrawableStaticMeshComponent* m_mesh = nullptr;
        DrawableSkinnedMeshComponent* m_skinningMesh = nullptr;
        SMaterial* m_material_clone = nullptr;
        // SMaterial* material (Override)
        bool m_disableShadow = false;
    };
}