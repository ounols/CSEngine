#include "../Util/Matrix.h"
#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/AssetsDef.h"
#include "CameraMgr.h"
#include "../Util/Render/ShaderUtil.h"

#ifdef __linux__

#include <iostream>

#endif

using namespace CSE;

OGLMgr::OGLMgr() : m_projectionRatio(-1) {

}


OGLMgr::~OGLMgr() {
    releaseBuffers();
}


void OGLMgr::setShaderProgram() {


    SResource::Create<GLProgramHandle>("PBR.shader");

}


void OGLMgr::setBuffers() {

    //렌더버퍼 생성
    glGenRenderbuffers(1, &m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

    //깊이버퍼 생성 및 바인딩(옮김)
    //glGenRenderbuffers(1, &m_depthRenderbuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);

    //프레임버퍼 생성
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_depthRenderbuffer);

    // 렌더버퍼 바인딩
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);

#ifndef PLATFORM_IOS
    //프레임버퍼 바인딩
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}


void OGLMgr::setupEGLGraphics(GLuint width, GLuint height) {

    m_width = width;
    m_height = height;

    setBuffers();
    setShaderProgram();
    setProjectionRatio();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    //glDepthRangef(0.0,1.0);
}


void OGLMgr::setProjectionRatio() {
    if (m_width > m_height)
        m_projectionRatio = (GLfloat) m_width / (GLfloat) m_height;
    else
        m_projectionRatio = (GLfloat) m_height / (GLfloat) m_width;


    CameraMgr::getInstance()->SetProjectionRatio(m_projectionRatio);

}


void OGLMgr::Render(float elapsedTime) {

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ////VBO 언바인딩
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_TEXTURE_2D);


}


void OGLMgr::ResizeWindow(GLuint width, GLuint height) {

    if (m_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }

    // Create the depth buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    /* Protect against a divide by zero */
    if (height == 0) {
        height = 1;
    }

    /* Setup our viewport. */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    m_width = width;
    m_height = height;

    setProjectionRatio();

}


void OGLMgr::releaseBuffers() {

    glDeleteFramebuffers(1, &m_framebuffer);
    m_framebuffer = 0;
    glDeleteRenderbuffers(1, &m_colorRenderbuffer);
    m_colorRenderbuffer = 0;

    if (m_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
