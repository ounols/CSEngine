#include "RenderMgr.h"
#include "LightMgr.h"
#include "EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/SEnvironmentMgr.h"
#include "../Util/Render/SGBuffer.h"
#include "CameraMgr.h"
#include "../Component/RenderComponent.h"
#include "../Util/Settings.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/ShaderUtil.h"

using namespace CSE;

CameraMgr* cameraMgr = nullptr;
LightMgr* lightMgr = nullptr;

int mainTextureId = -1;
STexture* mainTexture = nullptr;

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

    m_mainBuffer = new SFrameBuffer();
    m_mainBuffer->GenerateFramebuffer(SFrameBuffer::PLANE);
    m_mainBuffer->GenerateRenderbuffer(SFrameBuffer::RENDER, (int)*m_width, (int)*m_height, GL_RGB);
    m_mainBuffer->RasterizeFramebuffer();

    m_mainProgramHandle = SResource::Create<GLProgramHandle>(Settings::GetDefaultMainBufferShaderID());
    mainTextureId = m_mainProgramHandle->UniformLocation("main.albedo")->id;
}

void RenderMgr::SetViewport() {
    for (const auto& gbufferPair : m_gbufferLayer) {
        const auto& gbuffer = gbufferPair.second;
        gbuffer->ResizeGBuffer((int)*m_width, (int)*m_height);
    }

    m_mainBuffer->ResizeFrameBuffer((int)*m_width, (int)*m_height);
    mainTexture = m_mainBuffer->GetTexture(0);
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

    m_mainBuffer->AttachFrameBuffer(GL_READ_FRAMEBUFFER);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, *m_width, *m_height, 0, 0, *m_width, *m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RenderMgr::RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    const bool hasFrameBuffer = frameBuffer != nullptr;
    const auto& lightPassHandle = gbuffer.GetLightPassHandle();

    const int bufferWidth = hasFrameBuffer ? frameBuffer->GetWidth() : *m_width;
    const int bufferHeight = hasFrameBuffer ? frameBuffer->GetHeight() : *m_height;

    /** ======================
     *  1. Geometry Pass
     */
    gbuffer.AttachGeometryFrameBuffer();
    glViewport(0, 0, *m_width, *m_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_geometryHandle->Program);
    const auto& renderLayer = gbuffer.GetRendersLayer();

    glEnableVertexAttribArray(m_geometryHandle->Attributes.Position);
    glEnableVertexAttribArray(m_geometryHandle->Attributes.Normal);
    glEnableVertexAttribArray(m_geometryHandle->Attributes.TextureCoord);
    glEnableVertexAttribArray(m_geometryHandle->Attributes.Weight);
    glEnableVertexAttribArray(m_geometryHandle->Attributes.JointId);

    for (const auto& render : renderLayer) {
        if (render == nullptr) continue;
        if (!render->isRenderActive) continue;

        render->SetMatrix(cameraMatrix);
        render->Render();
    }

    glDisableVertexAttribArray(m_geometryHandle->Attributes.Position);
    glDisableVertexAttribArray(m_geometryHandle->Attributes.Normal);
    glDisableVertexAttribArray(m_geometryHandle->Attributes.TextureCoord);
    glDisableVertexAttribArray(m_geometryHandle->Attributes.Weight);
    glDisableVertexAttribArray(m_geometryHandle->Attributes.JointId);

//    std::string save_str = CSE::AssetsPath() + "test.bmp";
//    saveScreenshot(save_str.c_str());
    /** ======================
     *  2. Light Pass
     */
    if(frameBuffer == nullptr) {
        m_mainBuffer->AttachFrameBuffer();
    }
    else {
        frameBuffer->AttachFrameBuffer();
    }
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
    gbuffer.AttachGeometryFrameBuffer(GL_READ_FRAMEBUFFER);
    if(frameBuffer == nullptr) {
        m_mainBuffer->AttachFrameBuffer(GL_DRAW_FRAMEBUFFER);
    }
    else {
        frameBuffer->AttachFrameBuffer(GL_DRAW_FRAMEBUFFER);
    }

    glBlitFramebuffer(0, 0, *m_width, *m_height, 0, 0, bufferWidth, bufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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
    if(frameBuffer == nullptr) {
        m_mainBuffer->AttachFrameBuffer();
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
            customHandlerID = (int)lightMgr->GetShadowHandle()->Program;
        }
        frameBuffer->AttachFrameBuffer();
    }
    OrderRenderLayer orderRenderLayer(m_rendersLayer.begin(), m_rendersLayer.end());

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

            // Initialize various state.
            glEnableVertexAttribArray(handler.Attributes.Position);
            glEnableVertexAttribArray(handler.Attributes.Normal);
            glEnableVertexAttribArray(handler.Attributes.TextureCoord);
            glEnableVertexAttribArray(handler.Attributes.Weight);
            glEnableVertexAttribArray(handler.Attributes.JointId);

            for (const auto& render : renderComp) {
                if (render == nullptr) continue;
                if (!render->isRenderActive) continue;

                render->SetMatrix(cameraMatrix);
                render->Render();
            }

            glDisableVertexAttribArray(handler.Attributes.Position);
            glDisableVertexAttribArray(handler.Attributes.Normal);
            glDisableVertexAttribArray(handler.Attributes.TextureCoord);
            glDisableVertexAttribArray(handler.Attributes.Weight);
            glDisableVertexAttribArray(handler.Attributes.JointId);
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
    int customHandlerID = custom_handler.Program;
    if(frameBuffer == nullptr) {
        m_mainBuffer->AttachFrameBuffer();
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
    glEnableVertexAttribArray(custom_handler.Attributes.Position);
    glEnableVertexAttribArray(custom_handler.Attributes.Weight);
    glEnableVertexAttribArray(custom_handler.Attributes.JointId);

    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto& shadowObject : render_objects) {
        if(!shadowObject->isRenderActive) continue;
        const auto& shadow_transform = static_cast<RenderComponent*>(shadowObject)->GetGameObject()->GetTransform();

        if(LightMgr::SHADOW_DISTANCE < vec3::Distance(cameraMatrix.cameraPosition, shadow_transform->m_position))
            continue;

        shadowObject->SetMatrix(cameraMatrix, &custom_handler);
        shadowObject->Render(&custom_handler);
    }
    glDisableVertexAttribArray(custom_handler.Attributes.Position);
    glDisableVertexAttribArray(custom_handler.Attributes.Weight);
    glDisableVertexAttribArray(custom_handler.Attributes.JointId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderMgr::Exterminate() {
    RenderContainer::Exterminate();
}

void RenderMgr::ResetBuffer(const CameraBase& camera) const {
    const auto& frameBuffer = camera.GetFrameBuffer();
    if(frameBuffer == nullptr) {
        m_mainBuffer->AttachFrameBuffer();
    }
    else {
        frameBuffer->AttachFrameBuffer();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
