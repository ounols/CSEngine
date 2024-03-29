#include "DeferredRenderGroup.h"
#include "RenderMgr.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SGBuffer.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/ShaderUtil.h"
#include "../../Util/Settings.h"
#include "../LightMgr.h"
#include "../../Component/CameraComponent.h"
#include "../EngineCore.h"

using namespace CSE;

DeferredRenderGroup::DeferredRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
    m_width = renderMgr.GetWidth();
    m_height = renderMgr.GetHeight();

    m_lightMgr = CORE->GetCore(LightMgr);
}

DeferredRenderGroup::~DeferredRenderGroup() = default;

void DeferredRenderGroup::RegisterObject(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    const auto& shaders = material->GetShaders();
    if (shaders == nullptr) return;
    auto* handle = const_cast<GLProgramHandle*>(shaders->GetHandle("deferred"));

    auto gbuffer = m_gbufferLayer[shaders];
    if (!gbuffer) {
        gbuffer = new SGBuffer();
        gbuffer->GenerateGBuffer(*m_width, *m_height);
        m_gbufferLayer[shaders] = gbuffer;
    }
    gbuffer->PushBackToLayer(object);
    gbuffer->BindLightPass(handle);
}

void DeferredRenderGroup::RemoveObjects(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    const auto& programLayer = m_gbufferLayer;
    auto it = std::find_if(programLayer.begin(), programLayer.end(), [&material](const auto& elem) {
        return elem.first == material->GetShaders();
    });
    if (it != programLayer.end()) {
        it->second->RemoveToLayer(object);
    }
}

void DeferredRenderGroup::RenderAll(const CameraBase& camera) const {
    for (const auto& gbuffer_pair : m_gbufferLayer) {
        const auto& shaders = gbuffer_pair.first;
        const auto& gbuffer = gbuffer_pair.second;
        RenderGbuffer(camera, *gbuffer, *shaders);
    }
}

void
DeferredRenderGroup::RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer,
                                   const SShaderGroup& shaders) const {
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
    const auto& renderLayer = gbuffer.GetRendersLayer();
    const auto& handle = shaders.GetGeometryHandle();
    glUseProgram(handle->Program);

    for (const auto& render : renderLayer) {
        if (render == nullptr) continue;
        if (!render->isRenderActive) continue;

        const auto& material = render->GetMaterial();
        material->AttachElement();
        render->SetMatrix(cameraMatrix, handle);
        render->Render(handle);
    }

    /** ======================
     *  2. Light Pass
     */
    const auto& deferredBuffer =
            frameBuffer == nullptr ? m_renderMgr->GetMainBuffer() : frameBuffer;
    deferredBuffer->AttachFrameBuffer();
    gbuffer.AttachLightPass();
    //Attach Light
    m_lightMgr->AttachLightToShader(lightPassHandle);
    m_lightMgr->AttachLightMapToShader(lightPassHandle, m_lightMgr->GetShadowCount());
    const auto layoutBegin = m_lightMgr->GetShadowCount() + m_lightMgr->GetLightMapCount();
    gbuffer.AttachLightPassTexture(layoutBegin);
    BindSourceBuffer(*deferredBuffer, *lightPassHandle, layoutBegin + 3);

    ShaderUtil::BindCameraToShader(*lightPassHandle, cameraMatrix.camera, cameraMatrix.cameraPosition,
                                   cameraMatrix.projection, cameraMatrix.camera);

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
