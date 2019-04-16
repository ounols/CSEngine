#include "JointComponent.h"
#include "../TransformComponent.h"

COMPONENT_CONSTRUCTOR(JointComponent), m_id(-1), m_animatedMatrix(mat4::Identity()), m_inverseTransformMatrix(mat4::Identity()) {
    
}

JointComponent::JointComponent(const JointComponent& src) : SComponent(src) {
    m_animatedMatrix = src.m_animatedMatrix;
    m_id = src.m_id;
    m_inverseTransformMatrix = src.m_inverseTransformMatrix;
}

JointComponent::~JointComponent() {

}

void JointComponent::Exterminate() {

}

void JointComponent::Init() {

}

void JointComponent::Tick(float elapsedTime) {

}


void JointComponent::SetAnimationMatrix(mat4 animation) {
    m_animatedMatrix = animation;
}

void JointComponent::calcInverseBindTransform(mat4 parentTransform) {
    mat4 bindTransform = m_localBindMatrix * parentTransform;
    m_inverseTransformMatrix = mat4::Invert(bindTransform);

    auto children = gameObject->GetChildren();
    for (auto child : children) {
        JointComponent* child_comp = child->GetComponent<JointComponent>();

        if(child_comp == nullptr) continue;

        child_comp->calcInverseBindTransform(bindTransform);
    }
}

SComponent* JointComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(JointComponent, clone);

    clone->m_inverseTransformMatrix = mat4(m_inverseTransformMatrix);
    clone->m_animatedMatrix = mat4(m_animatedMatrix);
    clone->m_localBindMatrix = mat4(m_localBindMatrix);
    clone->m_id = m_id;

    return clone;
}

void JointComponent::SetBindLocalMatrix(mat4 mat) {
    m_localBindMatrix = mat;
}
