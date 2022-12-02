#include "DeferredRenderGroup.h"
#include "RenderMgr.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SGBuffer.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Settings.h"
#include "../LightMgr.h"
#include "../../Component/CameraComponent.h"
#include "../EngineCore.h"

using namespace CSE;

DeferredRenderGroup::DeferredRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
    m_width = renderMgr.GetWidth();
    m_height = renderMgr.GetHeight();

    m_lightMgr = CORE->GetCore(LightMgr);

    m_geometryHandle = SResource::Create<GLProgramHandle>(Settings::GetDeferredGeometryPassShaderID());
}

DeferredRenderGroup::~DeferredRenderGroup() = default;

void DeferredRenderGroup::RegisterObject(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    const auto& lightPassHandle = material->GetLightPassHandle();
    if (lightPassHandle == nullptr) return;

    const auto& key = m_gbufferLayer.find(lightPassHandle);
    SGBuffer* gbuffer = nullptr;
    if (key == m_gbufferLayer.end()) {
        gbuffer = new SGBuffer();
        gbuffer->GenerateGBuffer(*m_width, *m_height);
        m_gbufferLayer[lightPassHandle] = gbuffer;
    } else {
        gbuffer = key->second;
    }
    gbuffer->PushBackToLayer(object);
    gbuffer->BindLightPass(lightPassHandle);
}

void DeferredRenderGroup::RemoveObjects(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    const auto& programLayer = m_gbufferLayer;
    auto handlerPair = programLayer.find(material->GetLightPassHandle());
    if (handlerPair != programLayer.end()) {
        auto& layerVector = handlerPair->second;
        handlerPair->second->RemoveToLayer(object);
    }
}

void DeferredRenderGroup::RenderAll(const CameraBase& camera) const {
    for (const auto& gbuffer_pair : m_gbufferLayer) {
        const auto& light_handle = gbuffer_pair.first;
        const auto& gbuffer = gbuffer_pair.second;
        RenderGbuffer(camera, *gbuffer);
    }
}

void DeferredRenderGroup::RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    const bool hasFrameBuffer = frameBuffer != nullptr;
    const auto& lightPassHandle = gbuffer.GetLightPassHandle();

    const int bufferWidth = hasFrameBuffer ? frameBuffer->GetWidth() : (int) *m_width;
    const int bufferHeight = hasFrameBuffer ? frameBuffer->GetHeight() : (int) *m_height;

    /** ======================
     *  1. Geometry Pass
     */
    gbuffer.AttachGeometryFrameBuffer();
    glViewport(0, 0, *m_width, *m_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_geometryHandle->Program);
    const auto& renderLayer = gbuffer.GetRendersLayer();

    for (const auto& render : renderLayer) {
        if (render == nullptr) continue;
        if (!render->isRenderActive) continue;

        render->SetMatrix(cameraMatrix);
        render->Render();
    }

    /** ======================
     *  2. Light Pass
     */
//    if(frameBuffer == nullptr) {
//        GetMainBuffer()->AttachFrameBuffer();
//    }
//    else {
//        frameBuffer->AttachFrameBuffer();
//    }
    const auto& deferredBuffer =
            frameBuffer == nullptr ? m_renderMgr->GetMainBuffer() : frameBuffer;
    deferredBuffer->AttachFrameBuffer();
    gbuffer.AttachLightPass();
    //Attach Light
    m_lightMgr->AttachLightToShader(lightPassHandle);
    m_lightMgr->AttachLightMapToShader(lightPassHandle, m_lightMgr->GetShadowCount());
    const auto layoutBegin = m_lightMgr->GetShadowCount() + m_lightMgr->GetLightMapCount();
    gbuffer.AttachLightPassTexture(layoutBegin);
    BindSourceBuffer(*deferredBuffer, *lightPassHandle, layoutBegin + 5);

    gbuffer.RenderLightPass();

    /** ======================
     *  3. Blit the depth buffer
     */
//    if (frameBuffer == nullptr) {
//        m_renderMgr->GetMainBuffer()->BlitFrameBuffer(*deferredBuffer);
//    } else {
//        frameBuffer->BlitFrameBuffer(*deferredBuffer);
//    }
//    gbuffer.AttachGeometryFrameBuffer(GL_READ_FRAMEBUFFER);
//    if(frameBuffer == nullptr) {
//        GetMainBuffer()->AttachFrameBuffer(GL_DRAW_FRAMEBUFFER);
//    }
//    else {
//        frameBuffer->AttachFrameBuffer(GL_DRAW_FRAMEBUFFER);
//    }
//
//    glBlitFramebuffer(0, 0, *m_width, *m_height, 0, 0, bufferWidth, bufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void DeferredRenderGroup::SetViewport() {
    for (const auto& gbufferPair : m_gbufferLayer) {
        const auto& gbuffer = gbufferPair.second;
        gbuffer->ResizeGBuffer((int) *m_width, (int) *m_height);
    }
}

void DeferredRenderGroup::Exterminate() {
    for (auto gbuffer_pair : m_gbufferLayer) {
        auto gbuffer = gbuffer_pair.second;
        SAFE_DELETE(gbuffer);
    }
    m_gbufferLayer.clear();
}
