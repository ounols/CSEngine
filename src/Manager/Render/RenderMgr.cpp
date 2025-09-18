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
#include "SdfRenderGroup.h"

using namespace CSE;

//TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
GLProgramHandle* postHandle = nullptr;

RenderMgr::RenderMgr() = default;

RenderMgr::~RenderMgr() {
    Exterminate();
}

void RenderMgr::Init() {
    m_cameraMgr = CORE->GetCore(CameraMgr);
    m_lightMgr = CORE->GetCore(LightMgr);

    m_width = SEnvironmentMgr::GetPointerWidth();
    m_height = SEnvironmentMgr::GetPointerHeight();

    InitBuffers((int) *m_width, (int) *m_height);
#ifdef CSE_SETTINGS_RENDER_FORWARD_SUPPORT
    m_forwardRenderGroup = new ForwardRenderGroup(*this);
#endif
#ifdef CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
    m_deferredRenderGroup = new DeferredRenderGroup(*this);
#endif
    m_depthOnlyRenderGroup = new DepthOnlyRenderGroup(*this);
#ifdef CSE_SETTINGS_RENDER_SDFGI_SUPPORT
    m_sdfRenderGroup = new SdfRenderGroup(*this);
#endif

    //TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
//    postHandle = SResource::Create<GLProgramHandle>("File:Shader/Post/dof.post");
}

void RenderMgr::SetViewport() {
#ifdef CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
    if (m_deferredRenderGroup != nullptr)
        static_cast<DeferredRenderGroup*>(m_deferredRenderGroup)->SetViewport();
#endif
    ResizeBuffers((int) *m_width, (int) *m_height);
}

void RenderMgr::Render() const {
    // 0. Render depth buffer for shadows.
    RenderShadows();

#ifdef CSE_SETTINGS_RENDER_SDFGI_SUPPORT
    // 1. Render SDF Map.
    RenderSdfMap();
#endif

    // 2. Render active sub cameras.
    RenderSubCameras();

    // 3. Main Render Buffer
    RenderMainCamera();
}

void RenderMgr::RenderShadows() const {
    const auto& lightObjects = m_lightMgr->GetAll();
    for (const auto& light: lightObjects) {
        if (!light->IsShadow()) continue;
        m_depthOnlyRenderGroup->RenderAll(*light);
    }
    m_lightMgr->RefreshShadowCount();
}

void RenderMgr::RenderSubCameras() const {
    const auto& cameraObjects = m_cameraMgr->GetAll();
    const auto& mainCamera = m_cameraMgr->GetCurrentCamera();

    for (const auto& camera: cameraObjects) {
        if (!camera->GetIsEnable() || camera == mainCamera || camera->GetFrameBuffer() == nullptr) continue;
        ResetBuffer(*camera);
        RenderAllGroup(*camera);
    }
}

void RenderMgr::RenderMainCamera() const {
    const auto& mainCamera = m_cameraMgr->GetCurrentCamera();
    if (mainCamera == nullptr) return;

    ResetBuffer(*mainCamera);
    RenderAllGroup(*mainCamera);

    //TODO: 포스트 프로세싱 테스트용 코드 반드시 제거 요망!
    const auto& mainBuffer = GetMainBuffer();
//    mainBuffer->PostFrameBuffer(postHandle, *mainCamera);
    mainBuffer->AttachFrameBuffer(GL_READ_FRAMEBUFFER);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_deviceBuffer);
    glBlitFramebuffer(0, 0, *m_width, *m_height, 0, 0, *m_width, *m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RenderMgr::RenderSdfMap() const {
    const auto& mainCamera = m_cameraMgr->GetCurrentCamera();
    if (mainCamera == nullptr) return;
    m_sdfRenderGroup->RenderAll(*mainCamera);
}

void RenderMgr::Exterminate() {
    RenderContainer::Exterminate();
}

void RenderMgr::ResetBuffer(const CameraBase& camera) const {
    auto frameBuffer = camera.GetFrameBuffer();
    if (frameBuffer == nullptr) {
        frameBuffer = GetMainBuffer();
    }
    frameBuffer->AttachFrameBuffer();
    camera.RenderBackground();
}

void RenderMgr::BindSdfMapUniforms(const GLProgramHandle& handle) const {
    static_cast<SdfRenderGroup*>(m_sdfRenderGroup)->BindShaderUniforms(handle);
}

int RenderMgr::BindSdfMapTextures(const GLProgramHandle& handle, int textureLayout) const {
    return static_cast<SdfRenderGroup*>(m_sdfRenderGroup)->BindShaderMap(handle, textureLayout);
}

void RenderMgr::RenderAllGroup(const CameraBase& camera) const {
#ifdef CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
    m_deferredRenderGroup->RenderAll(camera); // Deferred Render
#endif
#ifdef CSE_SETTINGS_RENDER_FORWARD_SUPPORT
    m_forwardRenderGroup->RenderAll(camera); // Forward Render
#endif
}
