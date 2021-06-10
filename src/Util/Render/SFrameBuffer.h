#pragma once

#include "STexture.h"


namespace CSE {

    class SFrameBuffer : public STexture {
    public:
        enum BufferType {
            RENDER = 0, DEPTH = 1, STENCIL = 2,
        };
    public:
        SFrameBuffer();
        ~SFrameBuffer();

        void InitFrameBuffer(BufferType type, int width, int height);
        void AttachFrameBuffer(int index = 0, int level = 0) const;
        void DetachFrameBuffer() const;
        void Exterminate() override;

        unsigned int GetRenderBufferID() const;
        unsigned int GetFrameBufferID() const;

        void SetBufferType(BufferType bufferType);

        int GetWidth() const;
        int GetHeight() const;

        BufferType GetBufferType() const;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        BufferType m_bufferType = RENDER;
        int m_attachmentGL = GL_COLOR_ATTACHMENT0;
        int m_width = 512;
        int m_height = 512;

        unsigned int m_rbo = 0;
        unsigned int m_fbo = 0;

        bool m_isTexture = true;

    };
}
