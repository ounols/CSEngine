#pragma once

#include "../../Object/SResource.h"
#include "../../OGLDef.h"

namespace CSE {

    class STexture;

    class SFrameBuffer : public SResource {
    public:
        enum BufferType {
            RENDER = 0, DEPTH = 1, STENCIL = 2,
        };
    private:
        struct BufferObject {
            BufferType type = RENDER;
            unsigned int renderbufferId = 0;
            STexture* texture = nullptr;
        };
    public:
        SFrameBuffer();
        ~SFrameBuffer();

        void GenerateRenderbuffer(BufferType type, int width, int height, int internalFormat);
        void GenerateTexturebuffer(BufferType type, int width, int height, int internalFormat,
                                   int index = 0, int level = 0);
        void DetachFrameBuffer() const;
        void Exterminate() override;

        int GetWidth() const;
        int GetHeight() const;


    protected:
        void Init(const AssetMgr::AssetReference* asset) override;\

    private:
        int GenerateAttachmentType(BufferType type) const;

    private:
        int m_width = 512;
        int m_height = 512;

        unsigned int m_fbo = 0;
        std::vector<BufferObject*> m_buffers;

    };
}
