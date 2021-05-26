#pragma once

#include "../../Object/SResource.h"


namespace CSE {

    class SFrameBuffer : public SResource {
    public:
        enum BufferType {
            RENDER = 0, DEPTH = 1, STENCIL = 2,
        };
    public:
        SFrameBuffer();
        ~SFrameBuffer();

        void Init(BufferType type, int width, int height);
        void AttachFrameBuffer() const;
        void DetachFrameBuffer() const;
        void Exterminate() override;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        BufferType m_type = RENDER;
        int m_width = 512;
        int m_height = 512;

        unsigned int m_rbo = 0;
        unsigned int m_fbo = 0;

        bool m_isTexture = true;

    };
}
