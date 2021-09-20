#pragma once

#include "../../Object/SResource.h"
#include "../../OGLDef.h"
#include "STexture.h"

namespace CSE {

    class STexture;

    class SFrameBuffer : public SResource {
    public:
        enum BufferType {
            RENDER = 0, DEPTH = 1, STENCIL = 2,
        };
        enum BufferDimension {
            PLANE = 0, CUBE = 1,
        };
        enum BufferStatus {
            NONE = 0, COLOR_ONLY = 1, DEPTH_ONLY = 2, MULTI = 3,
        };
    private:
        struct BufferObject {
            BufferType type = RENDER;
            unsigned int renderbufferId = 0;
            STexture* texture = nullptr;
        };
    public:
        SFrameBuffer();
        ~SFrameBuffer() override;

        void GenerateFramebuffer(BufferDimension dimension);
        unsigned int GenerateRenderbuffer(BufferType type, int width, int height, int internalFormat);
        STexture* GenerateTexturebuffer(BufferType type, int width, int height, int channel,
                                        int level = 0);
        void RasterizeFramebuffer();
        void AttachCubeBuffer(int index, int level = 0) const;
        void AttachFrameBuffer(int target = GL_FRAMEBUFFER) const;
        void DetachFrameBuffer() const;
        void Exterminate() override;

        int GetWidth() const;
        int GetHeight() const;

        BufferStatus GetBufferStatus() const;


    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        int GenerateAttachmentType(SFrameBuffer::BufferType type, bool isIncreaseAttachment = true) const;
        int GenerateInternalFormatType(int channel) const;

    private:
        int m_width = 512;
        int m_height = 512;
        BufferDimension m_dimension = PLANE;

        unsigned int m_fbo = 0;
        std::vector<BufferObject*> m_buffers;

        mutable BufferStatus m_bufferStatus = BufferStatus::NONE;
        mutable unsigned short m_colorAttachmentSize = 0;
    };
}
