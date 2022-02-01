#include "CoreBufferContainer.h"
#include "../Util/Render/SFrameBuffer.h"

using namespace CSE;

void CoreBufferContainer::InitBuffers(int width, int height) {
    if (m_mainBuffer != nullptr || m_deferredBuffer != nullptr || m_forwardBuffer != nullptr)
        ResizeBuffers(width, height);

    m_mainBuffer = new SFrameBuffer();
    m_mainBuffer->GenerateFramebuffer(SFrameBuffer::PLANE, width, height);
    m_mainBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGB);
    m_mainBuffer->RasterizeFramebuffer();

    m_deferredBuffer = new SFrameBuffer();
    m_deferredBuffer->SetName("__Deferred Framebuffer__");
    m_deferredBuffer->GenerateFramebuffer(SFrameBuffer::PLANE, width, height);
    m_deferredBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGB);
    m_deferredBuffer->GenerateTexturebuffer(SFrameBuffer::DEPTH, GL_DEPTH_COMPONENT);
    m_deferredBuffer->RasterizeFramebuffer();

    m_forwardBuffer = new SFrameBuffer();
    m_forwardBuffer->SetName("__Forward Framebuffer__");
    m_forwardBuffer->GenerateFramebuffer(SFrameBuffer::PLANE, width, height);
    m_forwardBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGB);
    m_forwardBuffer->GenerateTexturebuffer(SFrameBuffer::DEPTH, GL_DEPTH_COMPONENT);
    m_forwardBuffer->RasterizeFramebuffer();
}

void CoreBufferContainer::ResizeBuffers(int width, int height) {
    m_mainBuffer->ResizeFrameBuffer(width, height);
    m_deferredBuffer->ResizeFrameBuffer(width, height);
    m_forwardBuffer->ResizeFrameBuffer(width, height);
}
