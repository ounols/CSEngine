#include "JointComponent.h"
#include "../TransformComponent.h"
#include "../../Object/SResource.h"

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

    {
        mat4 m = m_inverseTransformMatrix;
        PRINT_VALUE(m_inverseTransformMatrix, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',
                m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',
                m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',
                m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);

    }
    {
        mat4 m = m_animatedMatrix;
        PRINT_VALUE(m_animatedMatrix, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',
                    m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',
                    m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',
                    m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);

    }
    {
        mat4 m = m_localBindMatrix;
        PRINT_VALUE(m_localBindMatrix, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',
                    m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',
                    m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',
                    m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);

    }

    PRINT_END("component");
}