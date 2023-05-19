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

//TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
GLProgramHandle* postHandle = nullptr;

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

    //TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
    postHandle = SResource::Create<GLProgramHandle>("File:Shader/SDF/sdf.post");
}

void RenderMgr::SetViewport() {
    if(m_deferredRenderGroup != nullptr)
        static_cast<DeferredRenderGroup*>(m_deferredRenderGroup)->SetViewport();
    ResizeBuffers((int)*m_width, (int)*m_height);
}

void RenderMgr::Render() const {
    // 1. Render depth buffer for shadows.
    RenderShadows();

    // 2. Render active sub cameras.
    RenderSubCameras();

    // 3. Main Render Buffer
    RenderMainCamera();
}

void RenderMgr::RenderShadows() const {
    const auto& lightObjects = lightMgr->GetAll();
    for (const auto& light : lightObjects) {
        if(!light->IsShadow()) continue;
        m_depthOnlyRenderGroup->RenderAll(*light);
    }
    lightMgr->RefreshShadowCount();
}

void RenderMgr::RenderSubCameras() const {
    const auto& cameraObjects = cameraMgr->GetAll();
    const auto& mainCamera = cameraMgr->GetCurrentCamera();

    for (const auto& camera : cameraObjects) {
        if(!camera->GetIsEnable() || camera == mainCamera || camera->GetFrameBuffer() == nullptr) continue;
        ResetBuffer(*camera);
        m_deferredRenderGroup->RenderAll(*camera); // Deferred Render
        m_forwardRenderGroup->RenderAll(*camera); // Forward Render
    }
}

void RenderMgr::RenderMainCamera() const {
    const auto& mainCamera = cameraMgr->GetCurrentCamera();
    if (mainCamera == nullptr) return;

    ResetBuffer(*mainCamera);
    m_deferredRenderGroup->RenderAll(*mainCamera); // Deferred Render
    m_forwardRenderGroup->RenderAll(*mainCamera); // Forward Render

    //TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
    const auto& mainBuffer = GetMainBuffer();
    mainBuffer->PostFrameBuffer(postHandle, *mainCamera);
    mainBuffer->AttachFrameBuffer(GL_READ_FRAMEBUFFER);
#ifdef IOS
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 1);
#else
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
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