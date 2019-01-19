#include "AnimatorComponent.h"

COMPONENT_CONSTRUCTOR(AnimatorComponent) {

}

AnimatorComponent::~AnimatorComponent() {

}

void AnimatorComponent::Init() {
}


void AnimatorComponent::Tick(float elapsedTime) {
    if(m_currentAnimation == nullptr) return;

    if(m_startTime == 0) {
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
    m_animationTime += elapsedTime - m_startTime;

    if(m_animationTime > m_currentAnimation->GetLength()) {
        m_animationTime = (int)m_animationTime % (int)m_currentAnimation->GetLength();
    }

    m_startTime = elapsedTime;
}



std::map<std::string, mat4> AnimatorComponent::calculateCurrentAnimationPose() {
    std::vector<KeyFrame> frames = getPreviousAndNextFrames();
    float progression = CalculateProgression(frames[0], frames[1]);
    return InterpolatePoses(frames[0], frames[1], progression);
}

void AnimatorComponent::applyPoseToJoints(std::map<std::string, mat4> currentPose, JointComponent* joint,
                                          mat4 parentTransform) {
    SGameObject* object = joint->GetGameObject();
    mat4 currentLocalTransform = currentPose[object->GetName()];
    mat4 currentTransform = parentTransform * currentLocalTransform;
    for (auto child : object->GetChildren()) {
        applyPoseToJoints(currentPose, child->GetComponent<JointComponent>(), currentTransform);
    }
    currentTransform *= joint->GetInverseTransformMatrix();
    joint->SetAnimationMatrix(currentTransform);
}

std::vector<KeyFrame> AnimatorComponent::getPreviousAndNextFrames() {
    auto allFrames = m_currentAnimation->GetKeyFrames();
    KeyFrame previousFrame = allFrames[0];
    KeyFrame nextFrame = allFrames[0];
    for (int i = 1; i < allFrames.size(); i++) {
        nextFrame = allFrames[i];
        if (nextFrame.GetTimeStamp() > m_animationTime) {
            break;
        }
        previousFrame = allFrames[i];
    }

    std::vector<KeyFrame> result;
    result.push_back(previousFrame);
    result.push_back(nextFrame);
    return result;
}

float AnimatorComponent::CalculateProgression(KeyFrame previous, KeyFrame next) {
    float totalTime = next.GetTimeStamp() - previous.GetTimeStamp();
    float currentTime = m_animationTime - previous.GetTimeStamp();
    return currentTime / totalTime;
}

std::map<std::string, mat4> AnimatorComponent::InterpolatePoses(KeyFrame previousFrame, KeyFrame nextFrame, float t) {
    std::map<std::string, mat4> currentPose;
    for (const auto& frame : previousFrame.GetJointKeyFrames()) {
        const auto jointName = frame.first;
        JointTransform previousTransform = frame.second;
        JointTransform nextTransform = nextFrame.GetJointKeyFrames().at(jointName);
        JointTransform currentTransform = JointTransform::Interpolate(t, previousTransform, nextTransform);
        currentPose[jointName] = currentTransform.GetLocalMatrix();
    }
    return currentPose;
}