//
// Created by ounols on 19. 2. 9.
//

#include "../../Animation/Joint.h"
#include "../../../Component/Animation/JointComponent.h"
#include "../../../Component/TransformComponent.h"
#include "DAEConvertSGameObject.h"

DAEConvertSGameObject::DAEConvertSGameObject() {

}

DAEConvertSGameObject::~DAEConvertSGameObject() {

}

SGameObject* DAEConvertSGameObject::GetJoints(SGameObject* parent, Joint* data) {

    if (parent == nullptr) return nullptr;

    SGameObject* jointObject = new SGameObject(data->GetName());
    jointObject->GetComponent<TransformComponent>()->SetMatrix(data->GetBindLocalTransform());
    JointComponent* joint = jointObject->CreateComponent<JointComponent>();
    joint->SetID(data->GetIndex());

    for (Joint* child : data->GetChildren()) {
        GetJoints(jointObject, child);
    }

    parent->AddChild(jointObject);

    return jointObject;
}
