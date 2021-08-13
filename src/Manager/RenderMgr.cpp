#include "RenderMgr.h"
#include "LightMgr.h"
#include "EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/SEnvironmentMgr.h"
#include "../Util/Render/SGBuffer.h"
#include "CameraMgr.h"
#include "../Component/RenderComponent.h"
// #include <iostream>

using namespace CSE;

CameraMgr* cameraMgr = nullptr;
LightMgr* lightMgr = nullptr;

RenderMgr::RenderMgr() {
    m_NoneCamera = mat4::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
}


RenderMgr::~RenderMgr() {
    Exterminate();
}


void RenderMgr::Init() {
    // Setting PBS Environment
    m_environmentMgr = new SEnvironmentMgr();
    m_environmentMgr->RenderPBREnvironment();
    // Setting Shadow Environment
    m_environmentMgr->InitShadowEnvironment();

    cameraMgr = CORE->GetCore(CameraMgr);
    lightMgr = CORE->GetCore(LightMgr);

    m_width = SEnvironmentMgr::GetPointerWidth();
    m_height = SEnvironmentMgr::GetPointerHeight();

}

void RenderMgr::SetViewport() {
    SAFE_DELETE(m_gbufferObject);
    if((*m_width) * (*m_height) > 0) {
        m_gbufferObject = new SGBuffer();
        m_gbufferObject->GenerateGBuffer(*m_width, *m_height);
    }
}

void RenderMgr::Render() const {
    // Render Order : Depth Buffers -> Render Buffers -> Main Render Buffer

    // 1. Render depth buffer for shadows.
    const auto& lightObjects = lightMgr->GetAll();
    const auto& shadowObjects = lightMgr->GetShadowObject();
    const auto& shadowEnvironment = m_environmentMgr->GetShadowEnvironment();
    for (const auto& light : lightObjects) {
        if(light->m_disableShadow) continue;
        RenderShadowInstance(*light, *shadowEnvironment, shadowObjects);
    }
    lightMgr->RefreshShadowCount();

    const auto& cameraObjects = cameraMgr->GetAll();
    const auto& mainCamera = cameraMgr->GetCurrentCamera();

    // 2. Render active sub cameras.
    for (const auto& camera : cameraObjects) {
        if(!camera->GetIsEnable() || camera == mainCamera || camera->GetFrameBuffer() == nullptr) continue;
        RenderInstance(*camera);
    }

    if(mainCamera == nullptr) return;
    // 3. Main Render Buffer
    RenderInstance(*mainCamera);

}

void RenderMgr::RenderGbuffer(const CameraBase& camera) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    if(frameBuffer == nullptr) {
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, (GLsizei) *m_width, (GLsizei) *m_height);
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
//            customHandlerID = m_environmentMgr->GetShadowEnvironment()->Program;
        }
//        frameBuffer->AttachFrameBuffer();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    m_gbufferObject->AttachFrameBuffer();
    glViewport(0, 0, (GLsizei) *m_width, (GLsizei) *m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(NULL);
}

void RenderMgr::RenderInstance(const CameraBase& camera, const GLProgramHandle* custom_handler) const {

    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    int customHandlerID = custom_handler != nullptr ? custom_handler->Program : -1;
    if(frameBuffer == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (GLsizei) *m_width, (GLsizei) *m_height);
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
            customHandlerID = m_environmentMgr->GetShadowEnvironment()->Program;
        }
        frameBuffer->AttachFrameBuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (GLsizei) *m_width, (GLsizei) *m_height);
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferStatus() == SFrameBuffer::DEPTH_ONLY) {
            customHandlerID = m_environmentMgr->GetShadowEnvironment()->Program;
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
    m_rendersLayer.clear();
    SAFE_DELETE(m_environmentMgr);
}