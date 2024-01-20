#include "CoreBufferContainer.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/STexture.h"

using namespace CSE;

void CoreBufferContainer::InitBuffers(int width, int height) {
    if (m_mainBuffer != nullptr) {
        ResizeBuffers(width, height);
        return;
    }

    m_mainBuffer = new SFrameBuffer();
    m_mainBuffer->GenerateFramebuffer(SFrameBuffer::PLANE, width, height);
    m_mainBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGB);
    m_mainBuffer->RasterizeFramebuffer();
}

void CoreBufferContainer::ResizeBuffers(int width, int height) {
    m_mainBuffer->ResizeFrameBuffer(width, height);
}
