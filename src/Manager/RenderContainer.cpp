#include "RenderContainer.h"
#include "../Util/Render/SFrameBuffer.h"
#include <iostream>

using namespace CSE;

RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}


void RenderContainer::Register(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto& layer = m_rendersLayer[orderLayer][handler];
    layer.push_back(object);

}


void RenderContainer::Remove(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;

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