#include "RenderMgr.h"
#include "LightMgr.h"
#include "EngineCore.h"
// #include <iostream>

using namespace CSE;

RenderMgr::RenderMgr() {
    m_NoneCamera = mat4::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
}


RenderMgr::~RenderMgr() {
    Exterminate();
}


void RenderMgr::Init() {
}


void RenderMgr::Render() const {

    auto cameraMgr = CORE->GetCore(CameraMgr);
    auto lightMgr = CORE->GetCore(LightMgr);

    CameraComponent* cameraComponent = cameraMgr->GetCurrentCamera();
    mat4 camera = (cameraComponent != nullptr) ?
                  cameraComponent->GetCameraMatrix() : m_NoneCamera;
    mat4 projection = (cameraComponent != nullptr) ?
                      cameraComponent->GetProjectionMatrix() : mat4::Identity();
    vec3 cameraPosition = cameraComponent->GetCameraPosition();

    OrderRenderLayer orderRenderLayer(m_rendersLayer.begin(), m_rendersLayer.end());

    for (const auto& orderLayerPair : orderRenderLayer) {
        const auto& orderLayer = orderLayerPair.second;
        ProgramRenderLayer programComp(orderLayer.begin(), orderLayer.end());

        for (const auto& programPair : programComp) {

            const auto& handler = *programPair.first;
            const auto& renderComp = programPair.second;

            if (programPair.first == nullptr) continue;
            if (renderComp.empty()) continue;

            glUseProgram(handler.Program);

            //Attach Light
            lightMgr->AttachLightToShader(&handler);


            for (const auto& render : renderComp) {
                if (render == nullptr) continue;
                if (!render->isRenderActive) continue;

                // Initialize various state.
                glEnableVertexAttribArray(handler.Attributes.Position);
                glEnableVertexAttribArray(handler.Attributes.Normal);
                glEnableVertexAttribArray(handler.Attributes.TextureCoord);
                glEnableVertexAttribArray(handler.Attributes.Weight);
                glEnableVertexAttribArray(handler.Attributes.JointId);

                render->SetMatrix(camera, cameraPosition, projection);
                render->Render();

                glDisableVertexAttribArray(handler.Attributes.Position);
                glDisableVertexAttribArray(handler.Attributes.Normal);
                glDisableVertexAttribArray(handler.Attributes.TextureCoord);
                glDisableVertexAttribArray(handler.Attributes.Weight);
                glDisableVertexAttribArray(handler.Attributes.JointId);
            }
        }
    }

    ////VBO 언바인딩
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void RenderMgr::Exterminate() {
    m_rendersLayer.clear();

}
