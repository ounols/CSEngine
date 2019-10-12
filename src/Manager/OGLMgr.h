#pragma once
#include "../OGLDef.h"
#include <string>


namespace CSE {

    class OGLMgr {
    public:
        OGLMgr();

        ~OGLMgr();

        void setShaderProgram();

        void setBuffers();

        void setupEGLGraphics(GLuint width, GLuint height);

        void Render(float elapsedTime);

        void ResizeWindow(GLuint width, GLuint height);

        void releaseBuffers();

    private:
        void setProjectionRatio();

    private:
        GLuint m_width, m_height;
        GLfloat m_projectionRatio;

        GLuint m_depthRenderbuffer = 0;
        GLuint m_colorRenderbuffer = 0;
        GLuint m_framebuffer = 0;
    };

}