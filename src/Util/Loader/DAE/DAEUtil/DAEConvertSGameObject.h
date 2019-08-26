//
// Created by ounols on 19. 2. 9.
//

#pragma once

#include "../../../Animation/Joint.h"
#include "../DAEAnimationLoader.h"
#include "../../../../Object/SGameObject.h"

class KeyFrame;
class JointTransform;

class DAEConvertSGameObject {
public:
    DAEConvertSGameObject();
    ~DAEConvertSGameObject();

    static SGameObject* CreateJoints(SGameObject* parent, Joint* data);
    static SGameObject* CreateAnimation(SGameObject* parent, SGameObject* mesh, AnimationData* animationData,
                                        std::string name);

private:
    static KeyFrame* CreateKeyFrame(KeyFrameData* data);
    static JointTransform* CreateTransform(JointTransformData* data);
};