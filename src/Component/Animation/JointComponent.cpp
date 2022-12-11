#include "JointComponent.h"
#include "../TransformComponent.h"
#include "../../Object/SResource.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(JointComponent), m_id(-1), m_animatedMatrix(mat4::Identity()), m_inverseTransformMatrix(mat4::Identity()) {
    
}

JointComponent::JointComponent(const JointComponent& src) : SComponent(src) {
    m_animatedMatrix = src.m_animatedMatrix;
    m_id = src.m_id;
    m_inverseTransformMatrix = src.m_inverseTransformMatrix;
}

JointComponent::~JointComponent() = default;

void JointComponent::Exterminate() {

}

void JointComponent::Init() {

}

void JointComponent::Tick(float elapsedTime) {

}


void JointComponent::SetAnimationMatrix(mat4&& animation) {
    m_animatedMatrix = animation;
}

void JointComponent::calcInverseBindTransform(const mat4& parentTransform) {
    mat4 bindTransform = m_localBindMatrix * parentTransform;
    m_inverseTransformMatrix = mat4::Invert(bindTransform);

    auto children = gameObject->GetChildren();
    for (const auto& child : children) {
        auto child_comp = child->GetComponent<JointComponent>();

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
    clone->m_animationJointId = m_animationJointId;

    return clone;
}

void JointComponent::SetBindLocalMatrix(const mat4& mat) {
    m_localBindMatrix = mat;
}

void JointComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {
    if(name_str == "m_inverseTransformMatrix") {
        SET_MAT4(m_inverseTransformMatrix)
    }
    else if(name_str == "m_animatedMatrix") {
        SET_MAT4(m_animatedMatrix)
    }
    else if(name_str == "m_localBindMatrix") {
        SET_MAT4(m_localBindMatrix)
    }
}

std::string JointComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE_MAT4(m_inverseTransformMatrix);
    PRINT_VALUE_MAT4(m_animatedMatrix);
    PRINT_VALUE_MAT4(m_localBindMatrix);

    PRINT_END("component");
}