//
// Created by ounols on 21. 2. 12..
//

#pragma once

#include "../OGLDef.h"

namespace CSE {
    enum SType {
        UNKNOWN,
        STRING,
        BOOL = GL_BOOL,
        FLOAT = GL_FLOAT,
        INT = GL_INT,

        VEC2 = GL_FLOAT_VEC2,
        VEC3 = GL_FLOAT_VEC3,
        VEC4 = GL_FLOAT_VEC4,
        MAT2 = GL_FLOAT_MAT2,
        MAT3 = GL_FLOAT_MAT3,
        MAT4 = GL_FLOAT_MAT4,

        QUATERNION,
        RESOURCE,
        TEXTURE,
        MATERIAL,

        COMPONENT,
        GAME_OBJECT,
    };
}


