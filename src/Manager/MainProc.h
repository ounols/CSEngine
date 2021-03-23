#pragma once

#include "../OGLDef.h"

namespace CSE {

    class MainProc {
    public:
        MainProc();

        ~MainProc();

        void Init(GLuint width, GLuint height);

        void ResizeWindow(GLuint width, GLuint height) const;

        void Update(float elapsedTime);

        void Render(float elapsedTime) const;

        void Exterminate();
    };

}