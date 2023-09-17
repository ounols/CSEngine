#include "RenderComponent.h"
#include "../Manager/Render/RenderMgr.h"
#include "../Manager/LightMgr.h"
#include "../Manager/EngineCore.h"
#include "../Util/Render/ShaderUtil.h"
#include "../Util/Render/SMaterial.h"
#include "../Util/Settings.h"
#include "TransformComponent.h"
#include "DrawableSkinnedMeshComponent.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(RenderComponent) {
    m_renderMgr = CORE->GetCore(RenderMgr);
    SetMaterial(nullptr);
}

RenderComponent::~RenderComponent() = default;

void RenderComponent::Exterminate() {
    if (material != nullptr) {
        const auto& mode = material->GetMode();
        if (m_renderMgr != nullptr) {
            m_renderMgr->Remove(this, (RenderContainer::RenderGroupMode) mode);
            m_renderMgr->Remove(this, RenderContainer::DEPTH_ONLY);
        }
    }
    SAFE_DELETE(m_material_clone);
    material = nullptr;
}

void RenderComponent::Init() {

    if (!m_disableShadow) {
        m_renderMgr->Register(this, RenderContainer::DEPTH_ONLY);
    }

    m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
    if (m_mesh != nullptr) {
        m_skinningMesh = dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh);
    }

    if (material == nullptr) {
        isEnable = isRenderActive = false;
    }

    isRenderActive = isEnable;
}

void RenderComponent::Tick(float elapsedTime) {

    if (m_mesh == nullptr) {
        m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
        if (m_mesh != nullptr) {
            m_skinningMesh = dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh);
        }
    }
}

void
RenderComponent::SetMatrix(const CameraMatrixStruct& cameraMatrixStruct, const GLProgramHandle* handle) {
    ShaderUtil::BindCameraToShader(*handle, cameraMatrixStruct.camera, cameraMatrixStruct.cameraPosition,
                                   cameraMatrixStruct.projection,
                                   static_cast<const TransformComponent*>(gameObject->GetTransform())->GetMatrix());
}

void RenderComponent::Render(const GLProgramHandle* handle) const {

    if (m_mesh == nullptr || m_material_clone == nullptr || gameObject->isPrefab()) return;

    SetJointMatrix(handle);
    ShaderUtil::BindAttributeToShader(*handle, m_mesh->GetMeshID());
}

void RenderComponent::SetIsEnable(bool is_enable) {
    SComponent::SetIsEnable(is_enable);

    isRenderActive = isEnable;
}

SComponent* RenderComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(RenderComponent, clone);

    clone->isRenderActive = isRenderActive;
    clone->SetMaterial(material);

    return clone;
}

void RenderComponent::SetJointMatrix(const GLProgramHandle* handle) const {
    ShaderUtil::BindSkinningDataToShader(*handle, m_mesh->GetMeshID(),
                                         m_skinningMesh != nullptr ?
                                         m_skinningMesh->GetJointMatrix() : std::vector<mat4>());
}

SMaterial* RenderComponent::GetMaterial() const {
    return m_material_clone;
}

void RenderComponent::SetMaterial(SMaterial* material) {
    auto renderMgr = CORE->GetCore(RenderMgr);
    if (this->material == nullptr)
        this->material = SResource::Create<SMaterial>(Settings::GetDefaultDeferredMaterialId());
    else {
        const auto& mode = this->material->GetMode();
        renderMgr->Remove(this, (RenderContainer::RenderGroupMode) mode);
        this->material = material;
    }

    const auto& mode = this->material->GetMode();
    renderMgr->Register(this, (RenderContainer::RenderGroupMode) mode);

    SAFE_DELETE(m_material_clone)
    m_material_clone = new SMaterial(this->material);
}

void RenderComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {
    if (name_str == "material") {
        SetMaterial(SResource::Create<SMaterial>(value[0]));
    }
}

std::string RenderComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_RES(material);

    PRINT_END("component");
}
