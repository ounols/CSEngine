#pragma once

#include "SComponent.h"
#include "../Util/Interface/TransformInterface.h"
#include "../Util/Matrix.h"


namespace CSE {

    class TransformComponent : public SComponent, public TransformInterface {
    public:
        TransformComponent();

        ~TransformComponent() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void Exterminate() override;

        SComponent* Clone(SGameObject* object) override;

        mat4 GetMatrix() const;

        vec3* GetPosition();

        vec3 GetScale() const;

        Quaternion GetRotation() const;

        void SetMatrix(const mat4& matrix);

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

    private:
        mat4 GetFinalMatrix() const;

    private:
        mat4 m_f_matrix;
        vec3 m_f_position;
        vec3 m_f_scale;
        Quaternion m_f_rotation;
    };

}