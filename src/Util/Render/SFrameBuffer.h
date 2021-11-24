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

        /**
         * This is the first function called when creating a framebuffer.
         * You specify the dimensions of the framebuffer through that function.
         * @param dimension Sets the dimensions of the framebuffer. The default is SFrameBuffer::PLANE.
         */
        void GenerateFramebuffer(BufferDimension dimension);
        /**
         * Creates a render buffer in a non-texture format.
         * @param type Specifies the type of buffer to create. The default is RENDER.
         * @param width Write the width of the render buffer. The width is fixed at that value first written.
         * @param height Writes the height of the render buffer. The height is fixed at that value first created.
         * @param internalFormat Sets the internal format for the render buffer. Format details can be found at <a href="https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml">this link</a>.
         * @return Returns the ID value of the created render buffer. If creation fails, 0 is returned.
         */
        unsigned int GenerateRenderbuffer(BufferType type, int width, int height, int internalFormat);
        /**
         * Creates a render buffer in texture format.
         * @param type Specifies the type of buffer to create. The default is RENDER.
         * @param width Write the width of the render buffer. The width is fixed at that value first written.
         * @param height Writes the height of the render buffer. The height is fixed at that value first created.
         * @param channel Sets the channel format for the render buffer. Format details can be found at <a href="https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml">this link</a>.
         * @param level Sets the texture mipmap level. Default is 0.
         * @return Returns the STexture of the created render texture buffer. If creation fails, nullptr is returned.
         */
        STexture* GenerateTexturebuffer(BufferType type, int width, int height, int channel,
                                        int level = 0);
        /**
         * Create all the render buffers to allocate to the framebuffer and proceed with rasterization.
         * You can use the framebuffer after calling the function.
         */
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
         * Get textures that exist in the framebuffer.
         * @param id texture id in framebuffer
         * @return if the corresponding index is an invalid texture type, nullptr is returned.
         */
        STexture* GetTexture(const char* id) const;
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
