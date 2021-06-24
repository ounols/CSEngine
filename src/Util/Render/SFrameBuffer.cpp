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
void SFrameBuffer::InitFrameBuffer(SFrameBuffer::BufferType type, int width, int height) {
    m_bufferType = type;
    SetBufferType(type);
    m_width = width;
    m_height = height;

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
}

void SFrameBuffer::AttachFrameBuffer(int index, int level) const {
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if(m_type == TEX_CUBE) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, m_attachmentGL, GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
                               m_texId, level);
    }
    else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, m_attachmentGL, GL_TEXTURE_2D, m_texId, level);
    }
}

void SFrameBuffer::DetachFrameBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int SFrameBuffer::GetRenderBufferID() const {
    return m_rbo;
}

unsigned int SFrameBuffer::GetFrameBufferID() const {
    return m_fbo;
}

void SFrameBuffer::SetBufferType(SFrameBuffer::BufferType bufferType) {
    m_bufferType = bufferType;
    switch (bufferType) {
        case RENDER:
            m_attachmentGL = GL_COLOR_ATTACHMENT0; break;
        case DEPTH:
            m_attachmentGL = GL_DEPTH_ATTACHMENT; break;
        case STENCIL:
        default:
            break;
    }
}

int SFrameBuffer::GetWidth() const {
    return m_width;
}

int SFrameBuffer::GetHeight() const {
    return m_height;
}

SFrameBuffer::BufferType SFrameBuffer::GetBufferType() const {
    return m_bufferType;
}
