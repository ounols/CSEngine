#pragma once

#include "SComponent.h"
#include "../Util/Interface/TransformInterface.h"
#include "../Util/Matrix.h"


class TransformComponent : public SComponent, public TransformInterface {
public:
    TransformComponent();

    ~TransformComponent();

    void Init() override;

    void Tick(float elapsedTime) override;

    void Exterminate() override;

    mat4 GetMatrix() const;

    vec3* GetPosition();
    vec3 GetScale() const;
    Quaternion GetRotation() const;

private:
    mat4 GetFinalMatrix() const;

private:
    mat4 m_f_matrix;
    vec3 m_f_position;
    vec3 m_f_scale;
    Quaternion m_f_rotation;
};

