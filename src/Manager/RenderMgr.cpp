#include "RenderMgr.h"
#include "LightMgr.h"
#include "EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
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
    lightMgr->RenderShadowMap(m_environmentMgr->GetShadowEnvironment());

    glViewport(0, 0, (GLsizei) m_width, (GLsizei) m_height);

    CameraComponent* cameraComponent = cameraMgr->GetCurrentCamera();
    if(cameraComponent == nullptr) return;
    const auto camera = cameraComponent->GetCameraMatrixStruct();

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
            glUseProgram(handler.Program);
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

                render->SetMatrix(camera);
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

void RenderMgr::RenderInstance(const GLProgramHandle* handle) const {

}


void RenderMgr::Exterminate() {
    m_rendersLayer.clear();
    SAFE_DELETE(m_environmentMgr);
}

