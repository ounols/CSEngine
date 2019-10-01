#include "AnimatorComponent.h"

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

    std::map<std::string, mat4> currentPose = calculateCurrentAnimationPose();
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


std::map<std::string, mat4> AnimatorComponent::calculateCurrentAnimationPose() {
    std::vector<KeyFrame*> frames = getPreviousAndNextFrames();
    float progression = CalculateProgression(frames[0], frames[1]);
    return InterpolatePoses(frames[0], frames[1], progression);
}

void AnimatorComponent::applyPoseToJoints(std::map<std::string, mat4>& currentPose, JointComponent* joint,
                                          mat4 parentTransform) {
    SGameObject* object = joint->GetGameObject();
    mat4 currentLocalTransform = currentPose[object->GetName()];
    mat4 currentTransform = currentLocalTransform * parentTransform;
    for (auto child : object->GetChildren()) {
        auto joint_component = child->GetComponent<JointComponent>();
        if (joint_component != nullptr)
            applyPoseToJoints(currentPose, joint_component, currentTransform);
    }
    currentTransform = joint->GetInverseTransformMatrix() * currentTransform;
    joint->SetAnimationMatrix(currentTransform);
}

std::vector<KeyFrame*> AnimatorComponent::getPreviousAndNextFrames() {
    auto allFrames = m_currentAnimation->GetKeyFrames();
    KeyFrame* previousFrame = allFrames[0];
    KeyFrame* nextFrame = allFrames[0];
    for (int i = 1; i < allFrames.size(); i++) {
        nextFrame = allFrames[i];
        if (nextFrame->GetTimeStamp() > m_animationTime) {
            break;
        }
        previousFrame = allFrames[i];
    }

    std::vector<KeyFrame*> result;
    result.push_back(previousFrame);
    result.push_back(nextFrame);
    return result;
}

float AnimatorComponent::CalculateProgression(KeyFrame* previous, KeyFrame* next) {
    float totalTime = next->GetTimeStamp() - previous->GetTimeStamp();
    float currentTime = m_animationTime - previous->GetTimeStamp();
    return currentTime / totalTime;
}

std::map<std::string, mat4> AnimatorComponent::InterpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float t) {
    std::map<std::string, mat4> currentPose;
    for (const auto& frame : previousFrame->GetJointKeyFrames()) {
        const auto jointName = frame.first;
        JointTransform* previousTransform = frame.second;
        JointTransform* nextTransform = nextFrame->GetJointKeyFrames()[jointName];
        JointTransform currentTransform = JointTransform::Interpolate(t, *previousTransform, *nextTransform);
        currentPose[jointName] = currentTransform.GetLocalMatrix();
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

