#include "RenderMgr.h"
#include "../LightMgr.h"
#include "../EngineCore.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/SEnvironmentMgr.h"
#include "../../Util/Render/SGBuffer.h"
#include "../CameraMgr.h"
#include "../../Component/RenderComponent.h"
#include "../../Util/Settings.h"
#include "../../Util/GLProgramHandle.h"

using namespace CSE;

CameraMgr* cameraMgr = nullptr;
LightMgr* lightMgr = nullptr;

RenderMgr::RenderMgr() = default;

RenderMgr::~RenderMgr() {
    Exterminate();
}

void RenderMgr::Init() {
    cameraMgr = CORE->GetCore(CameraMgr);
    lightMgr = CORE->GetCore(LightMgr);

    m_width = SEnvironmentMgr::GetPointerWidth();
    m_height = SEnvironmentMgr::GetPointerHeight();

    m_geometryHandle = SResource::Create<GLProgramHandle>(Settings::GetDeferredGeometryPassShaderID());
//    m_mainProgramHandle = SResource::Create<GLProgramHandle>(Settings::GetDefaultMainBufferShaderID());
    InitBuffers((int)*m_width, (int)*m_height);
}

void RenderMgr::SetViewport() {
    for (const auto& gbufferPair : m_gbufferLayer) {
        const auto& gbuffer = gbufferPair.second;
        gbuffer->ResizeGBuffer((int)*m_width, (int)*m_height);
    }
    ResizeBuffers((int)*m_width, (int)*m_height);
}

void RenderMgr::Render() const {
    // Render Order : Depth Buffers -> Sub Render Buffers -> Main Render Buffers
    // 1. Render depth buffer for shadows.
    const auto& lightObjects = lightMgr->GetAll();
    const auto& shadowObjects = lightMgr->GetShadowObject();
    const auto& shadowHandle = lightMgr->GetShadowHandle();
    for (const auto& light : lightObjects) {
        if(!light->IsShadow()) continue;
        RenderShadowInstance(*light, *shadowHandle, shadowObjects);
    }
    lightMgr->RefreshShadowCount();
    const auto& cameraObjects = cameraMgr->GetAll();
    const auto& mainCamera = cameraMgr->GetCurrentCamera();

    // 2. Render active sub cameras.
    for (const auto& camera : cameraObjects) {
        if(!camera->GetIsEnable() || camera == mainCamera || camera->GetFrameBuffer() == nullptr) continue;
        ResetBuffer(*camera);
        RenderGbuffers(*camera); // Deferred Render
        RenderInstances(*camera); // Forward Render
    }
    if(mainCamera == nullptr) return;

    // 3. Main Render Buffer
    ResetBuffer(*mainCamera);
    RenderGbuffers(*mainCamera); // Deferred Render
    RenderInstances(*mainCamera); // Forward Render

    /**
     * @Todo 포스트 프로세싱을 적용하기 위한 코드
     */
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0, 0, (GLsizei) *m_width, (GLsizei) *m_height);
//    glUseProgram(m_mainProgramHandle->Program);
//    mainTexture->Bind(mainTextureId, 0);
//
//    ShaderUtil::BindAttributeToPlane();

    GetMainBuffer()->AttachFrameBuffer(GL_READ_FRAMEBUFFER);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, *m_width, *m_height, 0, 0, *m_width, *m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RenderMgr::RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    const bool hasFrameBuffer = frameBuffer != nullptr;
    const auto& lightPassHandle = gbuffer.GetLightPassHandle();

    const int bufferWidth = hasFrameBuffer ? frameBuffer->GetWidth() : (int)*m_width;
    const int bufferHeight = hasFrameBuffer ? frameBuffer->GetHeight() : (int)*m_height;

    /** ======================
     *  1. Geometry Pass
     */
    gbuffer.AttachGeometryFrameBuffer();
    glViewport(0, 0, *m_width, *m_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_geometryHandle->Program);
    const auto& renderLayer = gbuffer.GetRendersLayer();

    for (const auto& render: renderLayer) {
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
    const auto& deferredBuffer = GetDeferredBuffer();
    deferredBuffer->AttachFrameBuffer();
    // TODO: Background 설정 따로 적용하도록 수정
    // TODO: 뒷배경 색상 적용 안됨
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gbuffer.AttachLightPass();
    //Attach Light
    lightMgr->AttachLightToShader(lightPassHandle);
    lightMgr->AttachLightMapToShader(lightPassHandle, lightMgr->GetShadowCount());
    gbuffer.AttachLightPassTexture(lightMgr->GetShadowCount() + lightMgr->GetLightMapCount());

    gbuffer.RenderLightPass();

    /** ======================
     *  3. Blit the depth buffer
     */
    if (frameBuffer == nullptr) {
        GetMainBuffer()->BlitFrameBuffer(*deferredBuffer);
    } else {
        frameBuffer->BlitFrameBuffer(*deferredBuffer);
    }
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

void RenderMgr::RenderGbuffers(const CameraBase& camera) const {
    for (const auto& gbuffer_pair : m_gbufferLayer) {
        const auto& light_handle = gbuffer_pair.first;
        const auto& gbuffer = gbuffer_pair.second;
        RenderGbuffer(camera, *gbuffer);
    }
}

void RenderMgr::RenderInstances(const CameraBase& camera, const GLProgramHandle* custom_handler) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    int customHandlerID = custom_handler != nullptr ? (int)custom_handler->Program : -1;
    OrderRenderLayer orderRenderLayer(m_rendersLayer.begin(), m_rendersLayer.end());
    if(frameBuffer == nullptr) {
        GetMainBuffer()->AttachFrameBuffer();
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
            customHandlerID = (int)lightMgr->GetShadowHandle()->Program;
        }
        frameBuffer->AttachFrameBuffer();
    }

    for (const auto& orderLayerPair : orderRenderLayer) {
        const auto& orderLayer = orderLayerPair.second;
        ProgramRenderLayer programComp(orderLayer.begin(), orderLayer.end());

        for (const auto& programPair : programComp) {
            const auto& handler = *programPair.first;
            const auto& renderComp = programPair.second;

            if (programPair.first == nullptr) continue;
            if (renderComp.empty()) continue;

            glUseProgram(customHandlerID < 0 ? handler.Program : customHandlerID);
            //Attach Light
            lightMgr->AttachLightToShader(&handler);

            for (const auto& render : renderComp) {
                if (render == nullptr) continue;
                if (!render->isRenderActive) continue;

                render->SetMatrix(cameraMatrix);
                render->Render();
            }
        }
    }

    ////VBO 언바인딩
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void RenderMgr::RenderShadowInstance(const CameraBase& camera, const GLProgramHandle& custom_handler,
                                     const std::list<SIRender*>& render_objects) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    int customHandlerID = (int)custom_handler.Program;
    if(frameBuffer == nullptr) {
        GetMainBuffer()->AttachFrameBuffer();
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
            customHandlerID = (int)lightMgr->GetShadowHandle()->Program;
        }
        frameBuffer->AttachFrameBuffer();
    }

    glUseProgram(customHandlerID);
    // Initialize various state.

    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto& shadowObject : render_objects) {
        if(!shadowObject->isRenderActive) continue;
        const auto& shadow_transform = static_cast<RenderComponent*>(shadowObject)->GetGameObject()->GetTransform();

        if(LightMgr::SHADOW_DISTANCE < vec3::Distance(cameraMatrix.cameraPosition, shadow_transform->m_position))
            continue;

        shadowObject->SetMatrix(cameraMatrix, &custom_handler);
        shadowObject->Render(&custom_handler);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderMgr::Exterminate() {
    RenderContainer::Exterminate();
}

void RenderMgr::ResetBuffer(const CameraBase& camera) const {
    auto frameBuffer = camera.GetFrameBuffer();
    if(frameBuffer == nullptr) {
        frameBuffer = GetMainBuffer();
    }
    frameBuffer->AttachFrameBuffer();
    camera.RenderBackground();
}