#pragma once
#include "../Vector.h"
#include "../Quaternion.h"

namespace CSE {

    struct TransformInterface {

        vec3 m_position;
        Quaternion m_rotation;
        vec3 m_scale;
    };
}