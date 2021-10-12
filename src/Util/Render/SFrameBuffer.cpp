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
        ReleaseBufferObject(buffer);
    }
}

// Init function for asset binding
void SFrameBuffer::Init(const AssetMgr::AssetReference* asset) {
}

void SFrameBuffer::GenerateFramebuffer(SFrameBuffer::BufferDimension dimension) {
    m_dimension = dimension;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

unsigned int SFrameBuffer::GenerateRenderbuffer(BufferType type, int width, int height, int internalFormat) {
    if(m_dimension == CUBE) return 0;

    auto buffer = new BufferObject();
    buffer->type = type;
    buffer->format = internalFormat;
    m_width = width;
    m_height = height;

    glGenRenderbuffers(1, &buffer->renderbufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer->renderbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GenerateInternalFormatType(internalFormat), width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GenerateAttachmentType(type, false), GL_RENDERBUFFER, buffer->renderbufferId);

    m_buffers.push_back(buffer);
    return buffer->renderbufferId;
}

STexture* SFrameBuffer::GenerateTexturebuffer(BufferType type, int width, int height, int channel, int level) {
    auto buffer = new BufferObject();
    buffer->type = type;
    buffer->format = channel;
    buffer->level = (short)level;
    m_width = width;
    m_height = height;
    buffer->texture = new STexture(static_cast<STexture::Type>(m_dimension));
    buffer->texture->InitTexture(width, height, channel, GenerateInternalFormatType(channel), GL_FLOAT);
    unsigned int texId = buffer->texture->GetTextureID();
//    if(level > 0)

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if(m_dimension == PLANE)  {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GenerateAttachmentType(type), GL_TEXTURE_2D, texId, level);
    }
    else {
        // TODO: 큐브맵 텍스쳐 바인딩 설정 필요
        GenerateAttachmentType(buffer->type, false);
    }

    m_buffers.push_back(buffer);
    return buffer->texture;
}

void SFrameBuffer::RasterizeFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    m_buffers.reserve(m_buffers.size());

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    if(m_colorAttachmentSize > 1) {
        std::vector<unsigned int> attachments;
        attachments.reserve(m_colorAttachmentSize);
        for (int i = 0; i < m_colorAttachmentSize; ++i) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glDrawBuffers(m_colorAttachmentSize, &attachments[0]);
    }

    if(m_bufferStatus == COLOR_ONLY) {
        GenerateRenderbuffer(SFrameBuffer::DEPTH, m_width, m_height, GL_DEPTH_COMPONENT);
        m_bufferStatus = MULTI;
    }
}

void SFrameBuffer::AttachCubeBuffer(int index, int level) const {
    if(m_dimension != CUBE) return;

    for (const auto& buffer : m_buffers) {
        const auto& texture = buffer->texture;
        if(texture == nullptr) continue;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GenerateAttachmentType(buffer->type, false),
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, texture->GetTextureID(), level);
    }
}

void SFrameBuffer::AttachFrameBuffer(int target) const {
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(target, m_fbo);
}

void SFrameBuffer::DetachFrameBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SFrameBuffer::ResizeFrameBuffer(int width, int height) {
    std::vector<BufferObject*> origBufferVector(m_buffers);

    m_buffers.clear();
    m_buffers.reserve(origBufferVector.size());
    m_colorAttachmentSize = 0;
    glDeleteFramebuffers(1, &m_fbo);
    GenerateFramebuffer(m_dimension);

    for (const auto& buffer : origBufferVector) {
        auto type = buffer->type;
        auto format = buffer->format;
        auto level = buffer->level;
        auto isTexture = buffer->texture != nullptr;

        ReleaseBufferObject(buffer);

        if(isTexture)
            GenerateTexturebuffer(type, width, height, format, level);
        else
            GenerateRenderbuffer(type, width, height, format);
    }
    RasterizeFramebuffer();
}

int SFrameBuffer::GetWidth() const {
    return m_width;
}

int SFrameBuffer::GetHeight() const {
    return m_height;
}

int SFrameBuffer::GenerateAttachmentType(SFrameBuffer::BufferType type, bool isIncreaseAttachment) const {
    switch (type) {
        case RENDER:
            if(m_bufferStatus == BufferStatus::NONE) m_bufferStatus = COLOR_ONLY;
            if(m_bufferStatus == BufferStatus::DEPTH_ONLY) m_bufferStatus = MULTI;
            if(isIncreaseAttachment)
                return GL_COLOR_ATTACHMENT0 + (++m_colorAttachmentSize) - 1;
            else
                return GL_COLOR_ATTACHMENT0 + m_colorAttachmentSize;
        case DEPTH:
            if(m_bufferStatus == BufferStatus::NONE) m_bufferStatus = DEPTH_ONLY;
            if(m_bufferStatus == BufferStatus::COLOR_ONLY) m_bufferStatus = MULTI;
            return GL_DEPTH_ATTACHMENT;
        case STENCIL:
        default:
            return GL_COLOR_ATTACHMENT0;
    }
}

int SFrameBuffer::GenerateInternalFormatType(int channel) const {
    switch (channel) {
        case GL_RG:
            return GL_RG16F;
        case GL_DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT32F;
        case GL_RGBA:
            return GL_RGBA16F;
        case GL_RGB:
        default:
            return GL_RGB16F;
    }
}

void SFrameBuffer::ReleaseBufferObject(const SFrameBuffer::BufferObject* bufferObject) {
    if(bufferObject->renderbufferId > 0) glDeleteRenderbuffers(1, &bufferObject->renderbufferId);
    if(bufferObject->texture != nullptr) {
        auto resMgr = CORE->GetCore(ResMgr);
        resMgr->Remove(bufferObject->texture);
    }
    delete bufferObject;
}

SFrameBuffer::BufferStatus SFrameBuffer::GetBufferStatus() const {
    return m_bufferStatus;
}

STexture* SFrameBuffer::GetTexture(int index) const {
    if(index < 0 || index > m_buffers.size() - 1) return nullptr;

    auto texture = m_buffers[index]->texture;
    return texture;
}

unsigned int SFrameBuffer::GetRenderbufferID(int index) const {
    if(index < 0 || index > m_buffers.size() - 1) return 0;

    auto id = m_buffers[index]->renderbufferId;
    return id;
}
