#include "RenderMgr.h"
#include "LightMgr.h"
#include "EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/SEnvironmentMgr.h"
#include "CameraMgr.h"
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
}

void RenderMgr::SetViewport(int width, int height) {
    m_width = width;
    m_height = height;
}

void RenderMgr::Render() const {
    // Render Order : Depth Buffers -> Render Buffers -> Main Render Buffer

    lightMgr->RenderShadowMap(m_environmentMgr->GetShadowEnvironment());

    const auto& cameraObjects = cameraMgr->GetAll();
    const auto& mainCamera = cameraMgr->GetCurrentCamera();

    // Render active sub cameras.
    for (const auto& camera : cameraObjects) {
        if(!camera->GetIsEnable() || camera == mainCamera) continue;
        RenderInstance(*camera);
    }

    if(mainCamera == nullptr) return;
    // Main Render Buffer
    RenderInstance(*mainCamera);

}

void RenderMgr::RenderInstance(const CameraBase& camera, const GLProgramHandle* custom_handler) const {

    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    int customHandlerID = custom_handler != nullptr ? custom_handler->Program : -1;
    if(frameBuffer == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (GLsizei) m_width, (GLsizei) m_height);
    }
    else {
        // If the framebuffer is a depth buffer
        if(frameBuffer->GetBufferType() == SFrameBuffer::DEPTH) {
            customHandlerID = m_environmentMgr->GetShadowEnvironment()->Program;
        }
        frameBuffer->AttachFrameBuffer();
        glViewport(0, 0, (GLsizei) frameBuffer->GetWidth(), (GLsizei) frameBuffer->GetHeight());
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

            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
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

void RenderMgr::RenderShadowInstance(const CameraBase& camera, const GLProgramHandle* handle) const {

}

void RenderMgr::Exterminate() {
    m_rendersLayer.clear();
    SAFE_DELETE(m_environmentMgr);
}