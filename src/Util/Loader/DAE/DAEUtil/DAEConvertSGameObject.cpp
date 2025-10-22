#ifndef CSE_GLOBAL_SKINNED_ANIMATION_DISABLED
#include "DAEConvertSGameObject.h"
#include "../../../../Component/Animation/JointComponent.h"
#include "../../../../Component/Animation/AnimatorComponent.h"
#include "../../../../Manager/EngineCore.h"
#include "../../../../Object/SGameObjectFromSPrefab.h"

using namespace CSE;

DAEConvertSGameObject::DAEConvertSGameObject() = default;

DAEConvertSGameObject::~DAEConvertSGameObject() = default;

SGameObject* DAEConvertSGameObject::CreateJoints(SGameObject* parent, const Joint& data) {

    if (parent == nullptr) return nullptr;

    const auto& name = data.GetName();
    const auto& jointObject = new SGameObjectFromSPrefab(name);
    const auto& joint = jointObject->CreateComponent<JointComponent>();
    joint->SetID(data.GetIndex());
	const int jointIndex = CORE->GetCore(ResMgr)->GetStringHash(name);
	joint->SetAnimationJointId(jointIndex);
    joint->SetBindLocalMatrix(data.GetBindLocalTransform());

    const auto& children = data.GetChildren();
    for (const auto& child : children) {
        CreateJoints(jointObject, *child);
    }

    parent->AddChild(jointObject);

    return jointObject;
}

SGameObject* DAEConvertSGameObject::CreateAnimation(SGameObject* parent, JointComponent* rootJoint,
                                                    const AnimationData& animationData,
                                                    const std::string& name, Animation* animation) {
    if (parent == nullptr) return nullptr;

    const auto& animationObject = new SGameObjectFromSPrefab(parent->GetName() + "_animation");
    const auto& animator = animationObject->CreateComponent<AnimatorComponent>();

    std::list<KeyFrame*> frames;
    const unsigned int length = animationData.keyFrames.size();

    for (int i = 0; i < length; i++) {
        frames.push_back(CreateKeyFrame(*animationData.keyFrames[i]));
    }

    if (animation == nullptr)
        animation = new Animation();
    animation->LinkResource(name + ".prefab?animation");
    animation->SetKeyframe(animationData.lengthSeconds, frames);

    animator->PlayAnimation(animation);
    animator->SetRootJoint(rootJoint);

    parent->AddChild(animationObject);

    return animationObject;
}

KeyFrame* DAEConvertSGameObject::CreateKeyFrame(const KeyFrameData& data) {
    std::vector<JointTransform*> resultData;
    const auto& jointTransforms = data.jointTransforms;
    resultData.reserve(jointTransforms.size());

    for (const auto& jointData : jointTransforms) {
        JointTransform* jointTransform = CreateTransform(*jointData);
        resultData.push_back(jointTransform);
    }
    return new KeyFrame(data.time, std::move(resultData));
}

JointTransform* DAEConvertSGameObject::CreateTransform(const JointTransformData& data) {
    mat4 mat = data.jointLocalTransform;
    vec3&& translation = vec3(mat.w.x, mat.w.y, mat.w.z);
    Quaternion&& rotation = Quaternion::ToQuaternion(mat);
    return new JointTransform(translation, rotation);
}
#endif