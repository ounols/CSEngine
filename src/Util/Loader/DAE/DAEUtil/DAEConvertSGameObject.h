//
// Created by ounols on 19. 2. 9.
//

#pragma once

#include "../../../Animation/Joint.h"
#include "../DAEAnimationLoader.h"
#include "../../../../Object/SGameObject.h"
#include "../../../Animation/AnimationUtil.h"
#include "../../../Animation/Animation.h"

namespace CSE {

    class KeyFrame;

    class JointTransform;
    class JointComponent;

    class DAEConvertSGameObject {
    public:
        DAEConvertSGameObject();

        ~DAEConvertSGameObject();

        static SGameObject* CreateJoints(SGameObject* parent, Joint* data);

        static SGameObject* CreateAnimation(SGameObject* parent, JointComponent* rootJoint, AnimationData* animationData,
                                            const std::string& name, Animation* animation = nullptr);

    private:
        static KeyFrame* CreateKeyFrame(KeyFrameData* data);

        static JointTransform* CreateTransform(JointTransformData* data);
    };
}