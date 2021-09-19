#include "RenderContainer.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/SMaterial.h"
#include "../Util/Render/SGBuffer.h"
#include <iostream>

using namespace CSE;

RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}


void RenderContainer::Register(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;
    if(material->GetMode() == SMaterial::DEFERRED)
        return RegisterDeferred(object, material);

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto& layer = m_rendersLayer[orderLayer][handler];
    layer.push_back(object);

}

void RenderContainer::RegisterDeferred(SIRender* object, const SMaterial* material) {
    const auto& lightPassHandle = material->GetLightPassHandle();
    if(lightPassHandle == nullptr) return;

    const auto& key = m_gbufferLayer.find(lightPassHandle);
    SGBuffer* gbuffer;
    if(key == m_gbufferLayer.end()) {
        gbuffer = new SGBuffer();
        // TODO: 카메라에 맞게 능동적으로 프레임 버퍼를 수정할 수 있도록 수정이 필요함
        gbuffer->GenerateGBuffer(*m_width, *m_height);
        m_gbufferLayer[lightPassHandle] = gbuffer;
    }
    else {
        gbuffer = key->second;
    }
    gbuffer->PushBackToLayer(object);
    gbuffer->BindLightPass(lightPassHandle);
}

void RenderContainer::Remove(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;
    if(material->GetMode() == SMaterial::DEFERRED)
        return RemoveDeferred(object, material);

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
                handlerPair->second.erase(std::remove(layerVector.begin(), layerVector.end(), object), layerVector.end());
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

void RenderContainer::RemoveDeferred(SIRender* object, const SMaterial* material) {
    const auto& programLayer = m_gbufferLayer;
    auto handlerPair = programLayer.find(material->GetLightPassHandle());
    if (handlerPair != programLayer.end()) {
        auto& layerVector = handlerPair->second;
        handlerPair->second->RemoveToLayer(object);
    }
}

void RenderContainer::Exterminate() {
    m_rendersLayer.clear();
    for (auto gbuffer_pair : m_gbufferLayer) {
        auto gbuffer = gbuffer_pair.second;
        SAFE_DELETE(gbuffer);
    }
}