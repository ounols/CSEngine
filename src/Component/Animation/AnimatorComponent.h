#pragma once

#include "../SComponent.h"
#include "../DrawableSkinnedMeshComponent.h"
#include "../../Util/Animation/AnimationUtill.h"
#include "../../Util/Animation/Animation.h"

namespace CSE {

    class AnimatorComponent : public SComponent {
    public:
        AnimatorComponent();

        ~AnimatorComponent();

        void Init() override;

        void Tick(float elapsedTime) override;

        void Exterminate() override;

        SComponent* Clone(SGameObject* object) override;

        void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                           std::map<SComponent*, SComponent*> lists_comp) override;

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

        void SetMesh(DrawableSkinnedMeshComponent* mesh);
        // void SetAnimation(Animation* animation);

        void PlayAnimation(Animation* animation);


    private:
        void UpdateAnimationTime(float elapsedTime);

        std::map<int, mat4> calculateCurrentAnimationPose();

        void applyPoseToJoints(std::map<int, mat4>& currentPose, JointComponent* joint, mat4 parentTransform);

        std::vector<KeyFrame*> getPreviousAndNextFrames();

        float CalculateProgression(KeyFrame* previous, KeyFrame* next);

        std::map<int, mat4> InterpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float t);

    private:

        float m_animationTime = -1;
        float m_startTime = -1;

        DrawableSkinnedMeshComponent* m_entity = nullptr;
        Animation* m_currentAnimation = nullptr;
        //차후 애니메이션 컨트롤러도 제작

    };

}