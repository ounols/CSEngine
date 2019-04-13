//
// Created by ounols on 19. 2. 9.
//

#include <Component/MaterialComponent.h>
#include <Component/RenderComponent.h>
#include "../../Animation/Joint.h"
#include "../../../Component/Animation/JointComponent.h"
#include "../../../Component/Animation/AnimatorComponent.h"
#include "../../../Component/TransformComponent.h"
#include "DAEConvertSGameObject.h"

DAEConvertSGameObject::DAEConvertSGameObject() {

}

DAEConvertSGameObject::~DAEConvertSGameObject() {

}

SGameObject* DAEConvertSGameObject::CreateJoints(SGameObject* parent, Joint* data) {

    if (parent == nullptr) return nullptr;

    SGameObject* jointObject = new SGameObject(data->GetName());
    jointObject->GetComponent<TransformComponent>()->SetMatrix(data->GetBindLocalTransform());
    JointComponent* joint = jointObject->CreateComponent<JointComponent>();
    joint->SetID(data->GetIndex());
    joint->SetAnimationMatrix(data->GetBindLocalTransform());

    //============
    SGameObject* joint_render = new SGameObject(data->GetName() + "_renderer");
    jointObject->AddChild(joint_render);
    joint_render->GetTransform()->m_scale = vec3{ 4, 4, 4 };
    joint_render->CreateComponent<DrawableStaticMeshComponent>();
    joint_render->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*(ResMgr::getInstance()->GetSurfaceMesh(1)));
    joint_render->CreateComponent<MaterialComponent>();
    joint_render->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
    joint_render->CreateComponent<RenderComponent>();
    joint_render->GetComponent<RenderComponent>()->SetShaderHandle(0);
    //============

    for (Joint* child : data->GetChildren()) {
        CreateJoints(jointObject, child);
    }

    parent->AddChild(jointObject);

    return jointObject;
}

SGameObject* DAEConvertSGameObject::CreateAnimation(SGameObject* parent, SGameObject* mesh, AnimationData* animationData) {
    if(parent == nullptr) return nullptr;

    SGameObject* animationObject = new SGameObject(parent->GetName() + "_animation");
    AnimatorComponent* animator = animationObject->CreateComponent<AnimatorComponent>();

    std::vector<KeyFrame*> frames;
    int length = animationData->keyFrames.size();

    for(int i = 0; i < length; i++) {
        frames.push_back(CreateKeyFrame(animationData->keyFrames[i]));
    }

    Animation* animation = new Animation(animationData->lengthSeconds, frames);

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
