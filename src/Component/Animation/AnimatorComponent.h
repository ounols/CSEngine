#pragma once

#include "../SComponent.h"
#include "../DrawableSkinnedMeshComponent.h"
#include "../../Util/Animation/AnimationUtil.h"
#include "../../Util/Animation/Animation.h"

namespace CSE {

    class AnimatorComponent : public SComponent {
    public:
        COMPONENT_DEFINE_CONSTRUCTOR(AnimatorComponent);

        ~AnimatorComponent() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void Exterminate() override;

        SComponent* Clone(SGameObject* object) override;

        void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                           std::map<SComponent*, SComponent*> lists_comp) override;

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

        void SetRootJoint(JointComponent* mesh);
        // void SetAnimation(Animation* animation);

        void PlayAnimation(Animation* animation);


    private:
        void UpdateAnimationTime(float elapsedTime);

        std::vector<mat4> calculateCurrentAnimationPose() const;

        static void applyPoseToJoints(std::vector<mat4>& currentPose, JointComponent* joint, const mat4& parentTransform);

        std::vector<KeyFrame*> getPreviousAndNextFrames() const;

        float CalculateProgression(KeyFrame* previous, KeyFrame* next) const;

        static std::vector<mat4> InterpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float t);

    private:

        float m_animationTime = -1;
        float m_startTime = -1;

        JointComponent* m_rootJoint = nullptr;
        Animation* m_currentAnimation = nullptr;
        //차후 애니메이션 컨트롤러도 제작

    };

}