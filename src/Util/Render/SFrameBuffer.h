#pragma once

#include "../../Object/SResource.h"
#include "../../OGLDef.h"
#include "STexture.h"

namespace CSE {

    class STexture;

    /**
     * @see To use the SFrameBuffer class, you need to follow the steps below:
     *  1. Generate a framebuffer.
     *  @code{.cpp}
     *  m_frameBuffer->GenerateFramebuffer(SFrameBuffer::PLANE);
     *  @endcode
     *
     *  2. Generate buffers.
     *  @code{.cpp}
     *  m_frameBuffer->GenerateTexturebuffer(SFrameBuffer::DEPTH, width, height, GL_DEPTH_COMPONENT);
     *  @endcode
     *
     *  3. Rasterize the framebuffer.
     *  @code{.cpp}
     *  m_frameBuffer->RasterizeFramebuffer();
     *  @endcode
     */
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
            int format = 0;
            short level = 0;
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
        void ResizeFrameBuffer(int width, int height);
        void Exterminate() override;

        int GetWidth() const;
        int GetHeight() const;

        BufferStatus GetBufferStatus() const;
        /**
         * Get textures that exist in the framebuffer.
         * @param index buffer index in framebuffer
         * @return if the corresponding index is an invalid texture type, nullptr is returned.
         */
        STexture* GetTexture(int index) const;
        /**
         * Get renderbuffer ID that exist in the framebuffer.
         * @param index buffer index in framebuffer
         * @return if the corresponding index is an invalid buffer ID, 0 is returned.
         */
        unsigned int GetRenderbufferID(int index) const;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        int GenerateAttachmentType(SFrameBuffer::BufferType type, bool isIncreaseAttachment = true) const;
        int GenerateInternalFormatType(int channel) const;
        void ReleaseBufferObject(const SFrameBuffer::BufferObject* bufferObject);

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
