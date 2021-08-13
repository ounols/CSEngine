#pragma once

#include "SFrameBuffer.h"

namespace CSE {

    class STexture;

    class SGBuffer : public SFrameBuffer {
    public:
        SGBuffer();
        ~SGBuffer() override;

        void GenerateGBuffer(int width, int height);

    private:
        STexture* m_positionTexture = nullptr;
        STexture* m_normalTexture = nullptr;
        STexture* m_albedoTexture = nullptr;
        STexture* m_materialTexture = nullptr;
        unsigned int m_depthRbo = 0;
    };
}