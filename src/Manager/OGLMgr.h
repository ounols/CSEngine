#pragma once
#include "../OGLDef.h"
#include "Base/CoreBase.h"
#include "Base/RenderCoreBase.h"
#include <string>

namespace CSE {
    class OGLMgr : public CoreBase, public RenderCoreBase {
    public:
        OGLMgr();
        ~OGLMgr() override;

        void Init() override;

        void setBuffers();
        void setupEGLGraphics();

        void Render() const override;

        void ResizeWindow(GLuint width, GLuint height);
        void releaseBuffers();

    private:
        void setProjectionRatio(int width, int height);

    private:
        GLfloat m_projectionRatio;
        GLuint m_depthRenderbuffer = 0;
        GLuint m_colorRenderbuffer = 0;
        GLuint m_framebuffer = 0;
    };
}