#include "AnimatorComponent.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(AnimatorComponent) {

}


AnimatorComponent::~AnimatorComponent() {

}

void AnimatorComponent::Init() {
}


void AnimatorComponent::Tick(float elapsedTime) {
    if (m_currentAnimation == nullptr) return;

    if (m_startTime == 0) {
        m_startTime = elapsedTime;
    }
    UpdateAnimationTime(elapsedTime);

    std::map<int, mat4> currentPose = calculateCurrentAnimationPose();
    applyPoseToJoints(currentPose, m_entity->GetRootJoint(), mat4::Identity());

}


void AnimatorComponent::Exterminate() {
}

void AnimatorComponent::SetMesh(DrawableSkinnedMeshComponent* mesh) {
    m_entity = mesh;
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


std::map<int, mat4> AnimatorComponent::calculateCurrentAnimationPose() const {
    std::vector<KeyFrame*> frames = getPreviousAndNextFrames();
    const float progression = CalculateProgression(frames[0], frames[1]);
    return InterpolatePoses(frames[0], frames[1], progression);
}

void AnimatorComponent::applyPoseToJoints(std::map<int, mat4>& currentPose, JointComponent* joint,
                                          const mat4 parentTransform) {
    SGameObject* object = joint->GetGameObject();
    const mat4 currentLocalTransform = currentPose[joint->GetAnimationJointId()];
    mat4 currentTransform = currentLocalTransform * parentTransform;
    auto children = object->GetChildren();
    for (const auto& child : children) {
	    const auto joint_component = child->GetComponent<JointComponent>();
        if (joint_component != nullptr)
            applyPoseToJoints(currentPose, joint_component, currentTransform);
    }
    currentTransform = joint->GetInverseTransformMatrix() * currentTransform;
    joint->SetAnimationMatrix(currentTransform);
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

std::map<int, mat4> AnimatorComponent::InterpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float t) {
    std::map<int, mat4> currentPose;
	auto jointKeyFrames_prev = previousFrame->GetJointKeyFrames();
	auto jointKeyFrames_next = nextFrame->GetJointKeyFrames();
    for (const auto& frame : jointKeyFrames_prev) {
        const auto jointId = frame.first;
        JointTransform* previousTransform = frame.second;
        JointTransform* nextTransform = jointKeyFrames_next[jointId];
        JointTransform currentTransform = JointTransform::Interpolate(t, *previousTransform, *nextTransform);
		currentPose.insert(std::pair<int, mat4>(jointId, currentTransform.GetLocalMatrix()));
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
    AnimatorComponent* convert = dynamic_cast<AnimatorComponent*>(src);

    //Copy Components
    FIND_COMP_REFERENCE(m_entity, convert, DrawableSkinnedMeshComponent);

}

void AnimatorComponent::SetValue(std::string name_str, Arguments value) {
    if (name_str == "m_animationTime") {
        m_animationTime = std::stof(value[0]);
    } else if (name_str == "m_startTime") {
        m_startTime = std::stof(value[0]);
    } else if (name_str == "m_currentAnimation") {
        m_currentAnimation = SResource::Create<Animation>(value[0]);
    } else if (name_str == "m_entity") {
        m_entity = gameObject->GetComponentByID<DrawableSkinnedMeshComponent>(value[0]);
    }
}

std::string AnimatorComponent::PrintValue() const {

    PRINT_START("component");

    PRINT_VALUE(m_animationTime, m_animationTime);
    PRINT_VALUE(m_startTime, m_startTime);
    PRINT_VALUE(m_currentAnimation, ConvertSpaceStr(m_currentAnimation->GetID()));
    PRINT_VALUE(m_entity, ConvertSpaceStr(m_entity->GetGameObject()->GetID(m_entity)));


    PRINT_END("component");
}

