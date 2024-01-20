#include "../Util/GLProgramHandle.h"
#include "DrawableSkinnedMeshComponent.h"
#include "../OGLDef.h"

#define MeshComponent DrawableSkinnedMeshComponent

using namespace CSE;

MeshComponent::MeshComponent(SGameObject* l_gameObject) : DrawableStaticMeshComponent(l_gameObject) {
    COMPONENT_DERIVED_CLASS_TYPE(DrawableSkinnedMeshComponent);
}

MeshComponent::~MeshComponent() = default;

void MeshComponent::SetRootJoint(SGameObject* joint_object, int joint_size) {
    m_jointRoot = joint_object->GetComponent<JointComponent>();
    m_jointSize = joint_size;

    m_jointRoot->calcInverseBindTransform(mat4::Identity());
    // SetJointSize(joint_object);
}

JointComponent* DrawableSkinnedMeshComponent::GetRootJoint() {
    return m_jointRoot;
}


std::vector<mat4> DrawableSkinnedMeshComponent::GetJointMatrix() const {
    std::vector<mat4> jointMatrices;

    if (m_jointRoot == nullptr) return jointMatrices;

    jointMatrices.resize(m_jointSize);
    addJointsToVector(m_jointRoot, jointMatrices);
    return jointMatrices;
}


void DrawableSkinnedMeshComponent::addJointsToVector(JointComponent* headJoint, std::vector<mat4>& matrix) const {
    int index = headJoint->GetID();
    matrix[index] = headJoint->GetAnimationMatrix();

    const auto& children = headJoint->GetGameObject()->GetChildren();
    for (const auto& childJoint : children) {
        const auto& joint = childJoint->GetComponent<JointComponent>();

        if (joint == nullptr) continue;
        addJointsToVector(joint, matrix);
    }
}

void DrawableSkinnedMeshComponent::SetJointSize(SGameObject* joint_object) {
    if (joint_object->GetComponent<JointComponent>() == nullptr) return;
    m_jointSize++;
    auto children = joint_object->GetChildren();
    for (const auto& childJoint : children) {
        SetJointSize(childJoint);
    }
}

bool DrawableSkinnedMeshComponent::SetMesh(const SISurface& meshSurface) {
    return DrawableStaticMeshComponent::SetMesh(meshSurface);
}

SComponent* DrawableSkinnedMeshComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(DrawableSkinnedMeshComponent, clone);

    clone->m_meshId = m_meshId;
    clone->m_jointSize = m_jointSize;

    return clone;
}

void DrawableSkinnedMeshComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                                 std::map<SComponent*, SComponent*> lists_comp) {
    if (src == nullptr) return;
    auto* convert = static_cast<DrawableSkinnedMeshComponent*>(src);

    //Copy Components
    FIND_COMP_REFERENCE(m_jointRoot, convert, JointComponent);
}
