#pragma once
#include "../SComponent.h"
#include "../../Util/Matrix.h"

class JointComponent : public SComponent {
public:
    JointComponent();
    ~JointComponent();

    void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

    void SetAnimationMatrix(mat4 animation);
    mat4 GetAnimationMatrix() const {
        return m_animatedMatrix;
    }

    void SetID(int id) {
        m_id = id;
    }
    int GetID() const {
        return m_id;
    }

    mat4 GetInverseTransformMatrix() const {
        return m_inverseTransformMatrix;
    }
    void calcInverseBindTransform(mat4 parentTransform);

private:
    int m_id;
    mat4 m_animatedMatrix;
    mat4 m_inverseTransformMatrix;
};