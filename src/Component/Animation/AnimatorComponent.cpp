#include "AnimatorComponent.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(AnimatorComponent) {

}


AnimatorComponent::~AnimatorComponent() = default;

void AnimatorComponent::Init() {
}


void AnimatorComponent::Tick(float elapsedTime) {
    if (m_currentAnimation == nullptr) return;

    if (m_startTime == 0) {
        m_startTime = elapsedTime;
    }
    UpdateAnimationTime(elapsedTime);

    std::vector<mat4> currentPose = calculateCurrentAnimationPose();
    applyPoseToJoints(currentPose, m_rootJoint, mat4::Identity());

}


void AnimatorComponent::Exterminate() {
}

void AnimatorComponent::SetRootJoint(JointComponent* mesh) {
    m_rootJoint = mesh;
}

// void AnimatorComponent::SetAnimation(Animation* animation) {
//     m_animation = animation;
// }

void AnimatorComponent::PlayAnimation(Animation* animation) {
    m_currentAnimation = animation;
    m_animationTime = m_startTime = 0;
}

void AnimatorComponent::UpdateAnimationTime(float elapsedTime) {
    m_animationTime += (elapsedTime - m_startTime) / 1000.f;
    float length = m_currentAnimation->GetLength();

    if (m_animationTime > length) {
        m_animationTime -= (m_animationTime / length) * length;
    }

    m_startTime = elapsedTime;
}


std::vector<mat4> AnimatorComponent::calculateCurrentAnimationPose() const {
    std::vector<KeyFrame*> frames = getPreviousAndNextFrames();
    const float progression = CalculateProgression(frames[0], frames[1]);
    return InterpolatePoses(frames[0], frames[1], progression);
}

void AnimatorComponent::applyPoseToJoints(std::vector<mat4>& currentPose, JointComponent* joint,
                                          const mat4& parentTransform) {
    const auto& object = joint->GetGameObject();
    const int jointId = joint->GetAnimationJointId();
    const mat4& currentLocalTransform = currentPose[jointId];
    mat4&& currentTransform = currentLocalTransform * parentTransform;
    auto children = object->GetChildren();
    for (const auto& child : children) {
	    const auto& joint_component = child->GetComponent<JointComponent>();
        if (joint_component != nullptr)
            applyPoseToJoints(currentPose, joint_component, currentTransform);
    }
    currentTransform = joint->GetInverseTransformMatrix() * currentTransform;
    joint->SetAnimationMatrix(std::move(currentTransform));
}

std::vector<KeyFrame*> AnimatorComponent::getPreviousAndNextFrames() const {
    auto allFrames = m_currentAnimation->GetKeyFrames();
    KeyFrame* previousFrame = allFrames.front();
    KeyFrame* nextFrame = allFrames.front();
    allFrames.pop_front();

    for (const auto& frame : allFrames) {
        nextFrame = frame;
        if (nextFrame->GetTimeStamp() > m_animationTime) {
            break;
        }
        previousFrame = frame;
    }

    std::vector<KeyFrame*> result;
    result.reserve(2);
    result.push_back(previousFrame);
    result.push_back(nextFrame);
    return result;
}

float AnimatorComponent::CalculateProgression(KeyFrame* previous, KeyFrame* next) const {
    float totalTime = next->GetTimeStamp() - previous->GetTimeStamp();
    float currentTime = m_animationTime - previous->GetTimeStamp();
    return currentTime / totalTime;
}

std::vector<mat4> AnimatorComponent::InterpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float t) {
    const auto& jointKeyFrames_prev = previousFrame->GetJointKeyFrames();
	const auto& jointKeyFrames_next = nextFrame->GetJointKeyFrames();
    const auto jointSize = jointKeyFrames_prev.size();
    std::vector<mat4> currentPose;
    currentPose.resize(jointSize);

    for (unsigned short i = 0; i < jointSize; ++i) {
        const auto& prevTransform = jointKeyFrames_prev[i];
        const auto& nextTransform = jointKeyFrames_next[i];
        JointTransform&& currentTransform = JointTransform::Interpolate(t, *prevTransform, *nextTransform);
        currentPose[i] = std::move(currentTransform).GetLocalMatrix();
    }
    return currentPose;
}

SComponent* AnimatorComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(AnimatorComponent, clone);

    clone->m_animationTime = m_animationTime;
    clone->m_startTime = m_startTime;
    clone->m_currentAnimation = m_currentAnimation;

    return clone;
}

void AnimatorComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                      std::map<SComponent*, SComponent*> lists_comp) {
    if (src == nullptr) return;
    auto convert = static_cast<AnimatorComponent*>(src);

    //Copy Components
    FIND_COMP_REFERENCE(m_rootJoint, convert, JointComponent);

}

void AnimatorComponent::SetValue(std::string name_str, Arguments value) {
    if (name_str == "m_animationTime") {
        m_animationTime = std::stof(value[0]);
    } else if (name_str == "m_startTime") {
        m_startTime = std::stof(value[0]);
    } else if (name_str == "m_currentAnimation") {
        m_currentAnimation = SResource::Create<Animation>(value[0]);
    } else if (name_str == "m_rootJoint") {
        m_rootJoint = gameObject->GetComponentByID<JointComponent>(value[0]);
    }
}

std::string AnimatorComponent::PrintValue() const {

    PRINT_START("component");

    PRINT_VALUE(m_animationTime, m_animationTime);
    PRINT_VALUE(m_startTime, m_startTime);
    PRINT_VALUE(m_currentAnimation, ConvertSpaceStr(m_currentAnimation->GetID()));
    PRINT_VALUE(m_rootJoint, ConvertSpaceStr(m_rootJoint->GetGameObject()->GetID(m_rootJoint)));


    PRINT_END("component");
}

