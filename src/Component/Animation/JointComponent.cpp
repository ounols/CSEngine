#include "JointComponent.h"
#include "../TransformComponent.h"

COMPONENT_CONSTRUCTOR(JointComponent), m_id(-1), m_animatedMatrix(mat4::Identity()), m_inverseTransformMatrix(mat4::Identity()) {
    
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
    mat4 localBindTransform = static_cast<TransformComponent*>(gameObject->GetTransform())->GetMatrix();
    mat4 bindTransform = parentTransform * localBindTransform;
		m_inverseTransformMatrix = mat4::Invert(bindTransform);

        auto children = gameObject->GetChildren();
		for (auto child : children) {
            JointComponent* child_comp = child->GetComponent<JointComponent>();

            if(child_comp == nullptr) continue;

			child_comp->calcInverseBindTransform(bindTransform);
		}
}