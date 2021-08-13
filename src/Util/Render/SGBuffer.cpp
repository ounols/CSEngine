#include "SGBuffer.h"
#include "STexture.h"

using namespace CSE;

SGBuffer::SGBuffer() {

}

SGBuffer::~SGBuffer() {

}

void SGBuffer::GenerateGBuffer(int width, int height) {
    GenerateFramebuffer(SFrameBuffer::PLANE);

    m_positionTexture = GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_normalTexture = GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_albedoTexture = GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_materialTexture = GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_depthRbo = GenerateRenderbuffer(SFrameBuffer::DEPTH, width, height, GL_DEPTH_COMPONENT);

    RasterizeFramebuffer();
}
