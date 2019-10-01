//
// Created by ounols on 19. 2. 9.
//

#include "DAEConvertSGameObject.h"
#include "../../../Animation/Joint.h"
#include "../../../../Component/Animation/JointComponent.h"
#include "../../../../Component/Animation/AnimatorComponent.h"
#include "../../../../Component/TransformComponent.h"

DAEConvertSGameObject::DAEConvertSGameObject() {

}

DAEConvertSGameObject::~DAEConvertSGameObject() {

}

SGameObject* DAEConvertSGameObject::CreateJoints(SGameObject* parent, Joint* data) {

    if (parent == nullptr) return nullptr;

    SGameObject* jointObject = new SGameObject(data->GetName());
    JointComponent* joint = jointObject->CreateComponent<JointComponent>();
    joint->SetID(data->GetIndex());
    joint->SetBindLocalMatrix(data->GetBindLocalTransform());

    for (Joint* child : data->GetChildren()) {
        CreateJoints(jointObject, child);
    }

    parent->AddChild(jointObject);

    return jointObject;
}

SGameObject* DAEConvertSGameObject::CreateAnimation(SGameObject* parent, SGameObject* mesh,
                                                    AnimationData* animationData,
                                                    std::string name, Animation* animation) {
    if(parent == nullptr) return nullptr;

    SGameObject* animationObject = new SGameObject(parent->GetName() + "_animation");
    AnimatorComponent* animator = animationObject->CreateComponent<AnimatorComponent>();

    std::vector<KeyFrame*> frames;
    int length = animationData->keyFrames.size();

    for(int i = 0; i < length; i++) {
        frames.push_back(CreateKeyFrame(animationData->keyFrames[i]));
    }

    if(animation == nullptr)
        animation = new Animation();
    animation->LinkResource(name + ".prefab?animation");
    animation->SetKeyframe(animationData->lengthSeconds, frames);

    animator->PlayAnimation(animation);
    animator->SetMesh(mesh->GetComponent<DrawableSkinnedMeshComponent>());

    parent->AddChild(animationObject);

    return animationObject;
}

KeyFrame* DAEConvertSGameObject::CreateKeyFrame(KeyFrameData* data) {
    std::map<std::string, JointTransform*> map;
    for (auto jointData : data->jointTransforms) {
        JointTransform* jointTransform = CreateTransform(jointData);
        map[jointData->jointNameId] = jointTransform;
    }
    return new KeyFrame(data->time, map);
}

JointTransform* DAEConvertSGameObject::CreateTransform(JointTransformData* data) {
    mat4 mat = data->jointLocalTransform;
    vec3 translation = vec3(mat.w.x, mat.w.y, mat.w.z);
    Quaternion rotation = Quaternion::ToQuaternion(mat);
    return new JointTransform(translation, rotation);
}
