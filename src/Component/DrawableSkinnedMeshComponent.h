#pragma once

#include "DrawableStaticMeshComponent.h"
#include "Animation/JointComponent.h"

namespace CSE {

    class DrawableSkinnedMeshComponent : public DrawableStaticMeshComponent {
    public:
        DrawableSkinnedMeshComponent();

        ~DrawableSkinnedMeshComponent();

        SComponent* Clone(SGameObject* object) override;

        void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                           std::map<SComponent*, SComponent*> lists_comp) override;


        void SetRootJoint(SGameObject* joint_object, int joint_size);

        JointComponent* GetRootJoint();

        std::vector<mat4> GetJointMatrix() const;

        bool SetMesh(const SISurface& meshSurface) override;

    private:
        void addJointsToVector(JointComponent* headJoint, std::vector<mat4>& matrix) const;

        void SetJointSize(SGameObject* joint_object);

    private:
        JointComponent* m_jointRoot = nullptr;
        int m_jointSize = 0;


    };
}