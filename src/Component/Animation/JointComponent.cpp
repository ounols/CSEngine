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
        mat4 mat = mat4();
        mat.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));
        mat.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7]));
        mat.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11]));
        mat.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15]));
        m_inverseTransformMatrix = mat4(mat);
    }
    else if(name_str == "m_animatedMatrix") {
        mat4 mat = mat4();
        mat.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));
        mat.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7]));
        mat.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11]));
        mat.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15]));
        m_animatedMatrix = mat4(mat);
    }
    else if(name_str == "m_localBindMatrix") {
        mat4 mat = mat4();
        mat.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));
        mat.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7]));
        mat.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11]));
        mat.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15]));
        m_localBindMatrix = mat4(mat);
    }
}

std::string JointComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE_MAT4(m_inverseTransformMatrix);
    PRINT_VALUE_MAT4(m_animatedMatrix);
    PRINT_VALUE_MAT4(m_localBindMatrix);

    PRINT_END("component");
}