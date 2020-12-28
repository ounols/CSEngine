#pragma once

#include "../SComponent.h"
#include "../../Util/Matrix.h"

namespace CSE {

    class JointComponent : public SComponent {
    public:
        JointComponent();

        JointComponent(const JointComponent& src);

        ~JointComponent();

        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        SComponent* Clone(SGameObject* object) override;

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

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

		int GetAnimationJointId() const {
			return m_animationJointId;
		}

		void SetAnimationJointId(int animation_joint_id) {
			m_animationJointId = animation_joint_id;
		}
    	
    	mat4 GetInverseTransformMatrix() const {
            return m_inverseTransformMatrix;
        }

        void calcInverseBindTransform(mat4 parentTransform);

        void SetBindLocalMatrix(mat4 mat);

    private:
        int m_id;
		int m_animationJointId;
        mat4 m_animatedMatrix;
        mat4 m_inverseTransformMatrix;
        mat4 m_localBindMatrix;
    };
}