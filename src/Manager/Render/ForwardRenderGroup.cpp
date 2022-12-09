#include "ForwardRenderGroup.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Component/CameraComponent.h"
#include "RenderMgr.h"
#include "../LightMgr.h"
#include "../EngineCore.h"

using namespace CSE;


ForwardRenderGroup::ForwardRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
    m_lightMgr = CORE->GetCore(LightMgr);
}

ForwardRenderGroup::~ForwardRenderGroup() = default;

void ForwardRenderGroup::RegisterObject(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    if (material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto& layer = m_rendersLayer[orderLayer][handler];
    layer.push_back(object);
}

void ForwardRenderGroup::RemoveObjects(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    if (material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto orderLayerIter = m_rendersLayer.find(orderLayer);
    if (orderLayerIter != m_rendersLayer.end()) {
        auto& programLayer = orderLayerIter->second;
        auto handlerPair = programLayer.find(handler);
        if (handlerPair != programLayer.end()) {
            auto& layerVector = handlerPair->second;
            layerVector.erase(std::remove(layerVector.begin(), layerVector.end(), object),
                              layerVector.end());

            // 렌더링 객체가 제거된 뒤에도 공간이 비어 있는 경우, 그 공간을 제거한다.
            if (layerVector.empty()) programLayer.erase(handlerPair);
        }
        if (programLayer.empty()) m_rendersLayer.erase(orderLayerIter);
    }
}

void ForwardRenderGroup::RenderAll(const CameraBase& camera) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& cameraBuffer = camera.GetFrameBuffer();
    const auto& frameBuffer = cameraBuffer == nullptr
            ? m_renderMgr->GetMainBuffer() : cameraBuffer;
    OrderRenderLayer orderRenderLayer(m_rendersLayer.begin(), m_rendersLayer.end());
    frameBuffer->AttachFrameBuffer();

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
            m_lightMgr->AttachLightToShader(&handler);
            const auto layoutBegin = m_lightMgr->GetShadowCount() + m_lightMgr->GetLightMapCount();

            for (const auto& render : renderComp) {
                if (render == nullptr) continue;
                if (!render->isRenderActive) continue;

                const auto& material = render->GetMaterial();
                render->SetMatrix(cameraMatrix);
                BindSourceBuffer(*frameBuffer, handler, layoutBegin + material->GetTextureCount() + 1);
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
