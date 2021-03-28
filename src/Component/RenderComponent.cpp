#include "RenderComponent.h"
#include "../Manager/RenderMgr.h"
#include "../Manager/EngineCore.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(RenderComponent) {
    auto renderMgr = CORE->GetCore<RenderMgr>();
    renderMgr->Register(this);
    SetMaterial(nullptr);
}


RenderComponent::~RenderComponent() {}


void RenderComponent::Exterminate() {
//    RenderMgr::getInstance()->Remove(this);
    SAFE_DELETE(m_material_clone);
}


void RenderComponent::Init() {

    m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
    if (m_mesh != nullptr) {
        m_skinningMesh = dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh);
    }

    if(material == nullptr) {
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


void RenderComponent::SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection) {
    m_material_clone->SetCameraUniform(camera, cameraPosition, projection, gameObject->GetTransform());
}


void RenderComponent::Render() const {

    if (m_mesh == nullptr || m_material_clone == nullptr) return;

    AttachMaterials();
    SetJointMatrix();
    m_material_clone->SetAttribute(m_mesh->GetMeshID());
}


void RenderComponent::SetIsEnable(bool is_enable) {
    SComponent::SetIsEnable(is_enable);

    isRenderActive = isEnable;
}


void RenderComponent::AttachMaterials() const {

    //Set Materials

    if (m_material_clone == nullptr) {
        return;
    } else {
        m_material_clone->AttachElement();
    }


}

SComponent* RenderComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(RenderComponent, clone);

    clone->isRenderActive = isRenderActive;
    clone->SetMaterial(material);

    return clone;
}

void RenderComponent::SetJointMatrix() const {

    m_material_clone->SetSkinningUniform(m_mesh->GetMeshID(), m_skinningMesh != nullptr ?
    m_skinningMesh->GetJointMatrix() : std::vector<mat4>());
}

SMaterial* RenderComponent::GetMaterial() const {
    return m_material_clone;
}

void RenderComponent::SetMaterial(SMaterial* material) {
    auto renderMgr = CORE->GetCore<RenderMgr>();
    renderMgr->Remove(this);
    if(this->material == nullptr) this->material = SResource::Create<SMaterial>("File:Material/DefaultPBR.mat");
    else this->material = material;
    renderMgr->Register(this);

    SAFE_DELETE(m_material_clone)
    m_material_clone = new SMaterial(this->material);
}
