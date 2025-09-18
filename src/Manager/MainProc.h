#pragma once

#include "../OGLDef.h"
#ifndef __CSE_EDITOR__

namespace CSE {

    class MainProc {
    public:
        MainProc();

        ~MainProc();

        void Init(GLuint width, GLuint height);

        void ResizeWindow(GLuint width, GLuint height) const;

        void SetDeviceBuffer(unsigned int id);

        void Update(float elapsedTime);

        void Render(float elapsedTime) const;

        void GenerateCores();

        void Exterminate();
    };

}

#endif