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

#include "ForwardRenderGroup.h"
#include "DeferredRenderGroup.h"
#include "DepthOnlyRenderGroup.h"

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

    InitBuffers((int)*m_width, (int)*m_height);

    m_forwardRenderGroup = new ForwardRenderGroup(*this);
    m_deferredRenderGroup = new DeferredRenderGroup(*this);
    m_depthOnlyRenderGroup = new DepthOnlyRenderGroup(*this);
}

void RenderMgr::SetViewport() {
    if(m_deferredRenderGroup != nullptr)
        static_cast<DeferredRenderGroup*>(m_deferredRenderGroup)->SetViewport();
    ResizeBuffers((int)*m_width, (int)*m_height);
}

void RenderMgr::Render() const {
    // Render Order : Depth Buffers -> Sub Render Buffers -> Main Render Buffers
    // 1. Render depth buffer for shadows.
    const auto& lightObjects = lightMgr->GetAll();
    for (const auto& light : lightObjects) {
        if(!light->IsShadow()) continue;
        m_depthOnlyRenderGroup->RenderAll(*light);
    }
    lightMgr->RefreshShadowCount();
    const auto& cameraObjects = cameraMgr->GetAll();
    const auto& mainCamera = cameraMgr->GetCurrentCamera();

    // 2. Render active sub cameras.
    for (const auto& camera : cameraObjects) {
        if(!camera->GetIsEnable() || camera == mainCamera || camera->GetFrameBuffer() == nullptr) continue;
        ResetBuffer(*camera);
        m_deferredRenderGroup->RenderAll(*camera); // Deferred Render
        m_forwardRenderGroup->RenderAll(*camera); // Forward Render
    }
    if(mainCamera == nullptr) return;

    // 3. Main Render Buffer
    ResetBuffer(*mainCamera);
    m_deferredRenderGroup->RenderAll(*mainCamera); // Deferred Render
    m_forwardRenderGroup->RenderAll(*mainCamera); // Forward Render

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