#pragma once

#include "SComponent.h"
#include "../Util/Render/RenderInterfaces.h"
#include "CameraComponent.h"

namespace CSE {

    class DrawableStaticMeshComponent;
    class DrawableSkinnedMeshComponent;
    class LightMgr;
    class RenderMgr;
    class SMaterial;

    class RenderComponent : public SComponent, public SIRender {
    public:
        COMPONENT_DEFINE_CONSTRUCTOR(RenderComponent);

        ~RenderComponent() override;

        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        SComponent* Clone(SGameObject* object) override;


        void
        SetMatrix(const CameraMatrixStruct& cameraMatrixStruct,
                  const GLProgramHandle* handle) override;

        void Render(const GLProgramHandle* handle) const override;

        void SetIsEnable(bool is_enable) override;

        SMaterial* GetMaterial() const override;

        void SetMaterial(SMaterial* material);

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

    private:
        void SetJointMatrix(const GLProgramHandle* handle) const;

    private:
        LightMgr* m_lightMgr = nullptr;
        RenderMgr* m_renderMgr = nullptr;
        DrawableStaticMeshComponent* m_mesh = nullptr;
        DrawableSkinnedMeshComponent* m_skinningMesh = nullptr;
        SMaterial* m_material_clone = nullptr;
        // SMaterial* material (Override)
        bool m_disableShadow = false;
        bool m_isPrefab = false;
    };
}