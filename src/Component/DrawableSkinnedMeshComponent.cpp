#include "DrawableSkinnedMeshComponent.h"

#define MeshComponent DrawableSkinnedMeshComponent

DrawableSkinnedMeshComponent::DrawableSkinnedMeshComponent() {
    m_classType = "DrawableSkinnedMeshComponent";
}

MeshComponent::~MeshComponent() {

}

void MeshComponent::SetRootJoint(SGameObject* joint_object, int joint_size) {
    m_jointRoot = joint_object->GetComponent<JointComponent>();
    m_jointSize = joint_size;

    m_jointRoot->calcInverseBindTransform(mat4::Identity());
    // SetJointSize(joint_object);
}

JointComponent* DrawableSkinnedMeshComponent::GetRootJoint() {
    return m_jointRoot;
}



std::vector<mat4> DrawableSkinnedMeshComponent::GetJointMatrix() {
    std::vector<mat4> jointMatrices;

    if(m_jointRoot == nullptr) return jointMatrices;

    jointMatrices.resize(m_jointSize);
    addJointsToVector(m_jointRoot, jointMatrices);
    return jointMatrices;
}


void MeshComponent::CreateMeshBuffers(const SISurface& surface) {
    DrawableStaticMeshComponent::CreateMeshBuffers(surface);


}

void DrawableSkinnedMeshComponent::addJointsToVector(JointComponent* headJoint, std::vector<mat4> matrix) {
    int index = headJoint->GetID();
    matrix[index] = headJoint->GetAnimationMatrix();

    std::vector<SGameObject*> children = headJoint->GetGameObject()->GetChildren();
    for (SGameObject* childJoint : children) {
        auto joint = childJoint->GetComponent<JointComponent>();

        if(joint == nullptr) continue;
        addJointsToVector(joint, matrix);
    }
}

void DrawableSkinnedMeshComponent::SetJointSize(SGameObject* joint_object) {
    if(joint_object->GetComponent<JointComponent>() == nullptr) return;
    m_jointSize++;
    auto children = joint_object->GetChildren();
    for (SGameObject* childJoint : children) {
        SetJointSize(childJoint);
    }
}