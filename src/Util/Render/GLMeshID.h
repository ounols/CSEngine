//
// Created by ounol on 2021-03-18.
//
#pragma once

namespace CSE {
    struct GLMeshID {
        int m_vertexBuffer = -1;
        int m_indexBuffer = -1;
        int m_vertexSize = -1;
        int m_indexSize = -1;

        bool m_hasJoint = false;
    };
}
