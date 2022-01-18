//
// Created by 나 on 2021-05-26.
//

#include "SFrameBuffer.h"
#include "STexture.h"
#include "../../Manager/EngineCore.h"
#include "../../Manager/ResMgr.h"
#include "../Loader/XML/XML.h"
#include "../../PlatformDef.h"

#ifdef __CSE_DESKTOP__
#define CSE_GL_DEPTH_COMPONENT GL_DEPTH_COMPONENT32F
#define CSE_GL_RG GL_RG16F
#define CSE_GL_RGB GL_RGB16F
#define CSE_GL_RGBA GL_RGBA16F
#elif __CSE_ES__
#define CSE_GL_DEPTH_COMPONENT GL_DEPTH_COMPONENT16
#define CSE_GL_RG GL_RG8
#define CSE_GL_RGB GL_RGB8
#define CSE_GL_RGBA GL_RGBA8
#endif

using namespace CSE;

SFrameBuffer::SFrameBuffer() {
    SetUndestroyable(true);
}

SFrameBuffer::~SFrameBuffer() = default;

void SFrameBuffer::Exterminate() {
    glDeleteFramebuffers(1, &m_fbo);

    for(const auto& buffer : m_buffers) {
        ReleaseBufferObject(buffer);
    }
}

// Init function for asset binding
void SFrameBuffer::Init(const AssetMgr::AssetReference* asset) {
    const XNode* root = nullptr;

    try {
        root = XFILE(asset->path.c_str()).getRoot();
    }
    catch (int e) {
        SAFE_DELETE(root);
        return;
    }

    try {
        const XNode& cse_framebuffer = root->getChild("CSEFRAMEBUFFER");
        const XNode& info = cse_framebuffer.getChild("info");

        auto width_str = info.getAttribute("width").value;
        auto height_str = info.getAttribute("height").value;
        auto dimension_str = info.getAttribute("dimension").value;

        m_width = std::stoi(width_str);
        m_height = std::stoi(height_str);
        auto dimension = static_cast<BufferDimension>(std::stoi(dimension_str));
        GenerateFramebuffer(dimension);

        const XNode& buffers = cse_framebuffer.getChild("buffers");
        int index = 0;
        for (const auto& buffer_node : buffers.children) {
            auto type_str = buffer_node.getAttribute("type").value;
            auto format_str = buffer_node.getAttribute("format").value;
            auto isTexture_str = buffer_node.getAttribute("isTexture").value;

            auto type = static_cast<BufferType>(std::stoi(type_str));
            auto format = std::stoi(format_str);
            auto isTexture = std::stoi(isTexture_str) == 1;

            if(isTexture) {
                const auto& texture = GenerateTexturebuffer(type, m_width, m_height, format);
                const auto& textureName = "?Texture" + std::to_string(index);
                texture->SetName(GetName() + textureName);
                texture->SetID(GetID() + textureName);
            }
            else {
                GenerateRenderbuffer(type, m_width, m_height, format);
            }
            ++index;
        }
        RasterizeFramebuffer();
    }
    catch (int e) {
        SAFE_DELETE(root);
        auto resMgr = CORE->GetCore(ResMgr);
        resMgr->Remove(this);
    }
    SAFE_DELETE(root);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GenerateInternalFormat(internalFormat), width, height);

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
    buffer->texture->InitTexture(width, height, channel, GenerateInternalFormat(channel), GenerateInternalType(channel));
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

int SFrameBuffer::GenerateInternalFormat(int channel) const {
    switch (channel) {
        case GL_RG:
            return CSE_GL_RG;
        case GL_DEPTH_COMPONENT:
            return CSE_GL_DEPTH_COMPONENT;
        case GL_RGBA:
            return CSE_GL_RGBA;
        case GL_RGB:
        default:
            return CSE_GL_RGB;
    }
}

int SFrameBuffer::GenerateInternalType(int channel) const {
    switch (channel) {
        case GL_DEPTH_COMPONENT:
#ifdef __CSE_DESKTOP__
            return GL_FLOAT;
#elif __CSE_ES__
            return GL_UNSIGNED_INT;
#endif
        default:
#ifdef __CSE_DESKTOP__
            return GL_FLOAT;
#elif __CSE_ES__
            return GL_UNSIGNED_BYTE;
#endif
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

STexture* SFrameBuffer::GetTexture(const char* id) const {
    for (const auto& buffer : m_buffers) {
        const auto& texture = buffer->texture;
        if(texture == nullptr) continue;

        std::string bufferId = texture->GetID();
        if(bufferId == id) return texture;
    }
    return nullptr;
}

unsigned int SFrameBuffer::GetRenderbufferID(int index) const {
    if(index < 0 || index > m_buffers.size() - 1) return 0;

    auto id = m_buffers[index]->renderbufferId;
    return id;
}