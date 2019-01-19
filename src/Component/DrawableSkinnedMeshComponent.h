#pragma once
#include "DrawableStaticMeshComponent.h"
#include "Animation/JointComponent.h"

class DrawableSkinnedMeshComponent : public DrawableStaticMeshComponent {
public:
    DrawableSkinnedMeshComponent();
    ~DrawableSkinnedMeshComponent();

    void SetRootJoint(SGameObject* joint_object, int joint_size);
    JointComponent* GetRootJoint();
    std::vector<mat4> GetJointMatrix();
protected:
    void CreateMeshBuffers(const SISurface& surface);
    void addJointsToVector(JointComponent* headJoint, std::vector<mat4> matrix);
    void SetJointSize(SGameObject* joint_object);
private:
    JointComponent* m_jointRoot = nullptr;
    int m_jointSize = 0;
    // AnimatorComponent* m_animator = nullptr;

};