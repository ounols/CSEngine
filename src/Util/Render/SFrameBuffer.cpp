//
// Created by 나 on 2021-05-26.
//

#include "SFrameBuffer.h"
#include "STexture.h"
#include "../../Manager/EngineCore.h"
#include "../../Manager/ResMgr.h"

using namespace CSE;

SFrameBuffer::SFrameBuffer() {
    SetUndestroyable(true);
}

SFrameBuffer::~SFrameBuffer() {

}

void SFrameBuffer::Exterminate() {
    glDeleteFramebuffers(1, &m_fbo);

    for(const auto& buffer : m_buffers) {
        if(buffer->renderbufferId > 0) glDeleteRenderbuffers(1, &buffer->renderbufferId);
        if(buffer->texture != nullptr) {
            auto resMgr = CORE->GetCore(ResMgr);
            resMgr->Remove(buffer->texture);
        }
    }
}

// Init function for asset binding
void SFrameBuffer::Init(const AssetMgr::AssetReference* asset) {
}


void SFrameBuffer::GenerateRenderbuffer(SFrameBuffer::BufferType type, int width, int height, int internalFormat) {

    auto buffer = new BufferObject();
    buffer->type = type;
    m_width = width;
    m_height = height;

    glGenRenderbuffers(1, &buffer->renderbufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer->renderbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GenerateAttachmentType(type), GL_RENDERBUFFER, buffer->renderbufferId);
}

void SFrameBuffer::GenerateTexturebuffer(BufferType type, int width, int height, int internalFormat, int index,
                                         int level) {
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


int SFrameBuffer::GetWidth() const {
    return m_width;
}

int SFrameBuffer::GetHeight() const {
    return m_height;
}

int SFrameBuffer::GenerateAttachmentType(SFrameBuffer::BufferType type) const {
    switch (type) {
        case RENDER:
            return GL_COLOR_ATTACHMENT0 + m_buffers.size();
        case DEPTH:
            return GL_DEPTH_ATTACHMENT;
        case STENCIL:
        default:
            return GL_COLOR_ATTACHMENT0;
    }
}
