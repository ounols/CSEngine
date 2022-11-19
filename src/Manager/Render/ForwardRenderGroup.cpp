#include "ForwardRenderGroup.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Component/CameraComponent.h"
#include "RenderMgr.h"
#include "../LightMgr.h"
#include "../EngineCore.h"

using namespace CSE;

LightMgr* lightMgr = nullptr;

ForwardRenderGroup::ForwardRenderGroup(const RenderMgr& renderMgr) : SIRenderGroup(renderMgr) {
    lightMgr = CORE->GetCore(LightMgr);

}

ForwardRenderGroup::~ForwardRenderGroup() = default;


void ForwardRenderGroup::RegisterObject(SIRender* object) {
    const auto& material = object->GetMaterial();
    if (material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto& layer = m_rendersLayer[orderLayer][handler];
    layer.push_back(object);
}

void ForwardRenderGroup::RemoveObjects(SIRender* object) {
    const auto& material = object->GetMaterial();
    if (material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    //1. Order Render Layer Level
    auto orderLayerIter = m_rendersLayer.find(orderLayer);
    if (orderLayerIter != m_rendersLayer.end()) {
        {
            //2. Program Render Layer Level
            ProgramRenderLayer& programLayer = orderLayerIter->second;
            auto handlerPair = programLayer.find(handler);
            if (handlerPair != programLayer.end()) {
                auto& layerVector = handlerPair->second;
                handlerPair->second.erase(std::remove(layerVector.begin(), layerVector.end(), object),
                                          layerVector.end());
                //빈공간은 제거
                if (layerVector.empty()) {
                    programLayer.erase(handlerPair);
                }
            }
        }
        //빈공간은 제거
        if (orderLayerIter->second.empty()) {
            m_rendersLayer.erase(orderLayer);
        }
    }
}

void ForwardRenderGroup::RenderAll(const CameraBase& camera) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    OrderRenderLayer orderRenderLayer(m_rendersLayer.begin(), m_rendersLayer.end());
    if (frameBuffer == nullptr) {
        m_renderMgr->GetMainBuffer()->AttachFrameBuffer();
    } else {
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

            glUseProgram(handler.Program);
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

void ForwardRenderGroup::Exterminate() {
    m_rendersLayer.clear();
}
