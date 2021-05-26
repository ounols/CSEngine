//
// Created by 나 on 2021-05-26.
//

#include "SFrameBuffer.h"
#include "../../OGLDef.h"

using namespace CSE;

SFrameBuffer::SFrameBuffer() {
    SetUndestroyable(true);
}

SFrameBuffer::~SFrameBuffer() {

}

void SFrameBuffer::Exterminate() {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteRenderbuffers(1, &m_rbo);
}

// Init function for asset binding
void SFrameBuffer::Init(const AssetMgr::AssetReference* asset) {
}

// Init function for SFrameBuffer
void SFrameBuffer::Init(SFrameBuffer::BufferType type, int width, int height) {
    m_type = type;
    m_width = width;
    m_height = height;

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    glGenFramebuffers(1, &m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo);
}

void SFrameBuffer::AttachFrameBuffer() const {
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void SFrameBuffer::DetachFrameBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
