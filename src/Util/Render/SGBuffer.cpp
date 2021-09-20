#include "SGBuffer.h"
#include "STexture.h"
#include "SMaterial.h"
#include "SFrameBuffer.h"
#include "ShaderUtil.h"
#include "../GLProgramHandle.h"
#include "../../Manager/EngineCore.h"
#include "RenderInterfaces.h"

using namespace CSE;

ResMgr* resMgr = nullptr;


SGBuffer::SGBuffer() {
    resMgr = CORE->GetCore(ResMgr);
}

SGBuffer::~SGBuffer() {

}

void SGBuffer::GenerateGBuffer(int width, int height) {

    m_width = width;
    m_height = height;

    if(m_geometryFrameBuffer != nullptr) {
        resMgr->Remove(m_geometryFrameBuffer);
        SAFE_DELETE(m_geometryFrameBuffer);
    }
    m_geometryFrameBuffer = new SFrameBuffer();
    m_geometryFrameBuffer->GenerateFramebuffer(SFrameBuffer::PLANE);

    m_positionTexture = m_geometryFrameBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_normalTexture = m_geometryFrameBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_albedoTexture = m_geometryFrameBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_materialTexture = m_geometryFrameBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, width, height, GL_RGB);
    m_geometryFrameBuffer->RasterizeFramebuffer();


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
}

void SGBuffer::AttachGeometryFrameBuffer() const {
    if(m_geometryFrameBuffer == nullptr) return;
    m_geometryFrameBuffer->AttachFrameBuffer();
}

void SGBuffer::AttachGeometryFrameBuffer(int target) const {
    if(m_geometryFrameBuffer == nullptr) return;

    m_geometryFrameBuffer->AttachFrameBuffer(target);
}

void SGBuffer::ResizeGBuffer(int width, int height) {
    GenerateGBuffer(width, height);
}

void SGBuffer::ReleaseGBuffer() {

    if(m_geometryFrameBuffer != nullptr) {
        resMgr->Remove(m_geometryFrameBuffer);
        SAFE_DELETE(m_geometryFrameBuffer);
    }
}

void SGBuffer::BindLightPass(GLProgramHandle* lightPassHandle) {
    if(lightPassHandle == nullptr || m_lightPassHandle != nullptr) return;

    m_lightPassHandle = lightPassHandle;
    m_positonTextureId = m_lightPassHandle->UniformLocation("geo.position")->id;
    m_normalTextureId = m_lightPassHandle->UniformLocation("geo.normal")->id;
    m_albedoTextureId = m_lightPassHandle->UniformLocation("geo.albedo")->id;
    m_materialTextureId = m_lightPassHandle->UniformLocation("geo.material")->id;
}

void SGBuffer::AttachLightPass() const {
    glUseProgram(m_lightPassHandle->Program);
}

void SGBuffer::AttachLightPassTexture(int textureLayout) const {
    m_positionTexture->Bind(m_positonTextureId, textureLayout);
    m_normalTexture->Bind(m_normalTextureId, textureLayout + 1);
    m_albedoTexture->Bind(m_albedoTextureId, textureLayout + 2);
    m_materialTexture->Bind(m_materialTextureId, textureLayout + 3);


}

void SGBuffer::RenderLightPass() const {
    ShaderUtil::BindAttributeToLightPass();
}

const std::vector<SIRender*>& SGBuffer::GetRendersLayer() const {
    return m_rendersLayer;
}

void SGBuffer::PushBackToLayer(SIRender* render) {
    m_rendersLayer.push_back(render);
}

void SGBuffer::RemoveToLayer(SIRender* render) {
    m_rendersLayer.erase(std::remove(m_rendersLayer.begin(), m_rendersLayer.end(), render), m_rendersLayer.end());
}

int SGBuffer::GetWidth() const {
    return m_width;
}

int SGBuffer::GetHeight() const {
    return m_height;
}

GLProgramHandle* SGBuffer::GetLightPassHandle() const {
    return m_lightPassHandle;
}
