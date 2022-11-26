#include "RenderContainer.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SGBuffer.h"

using namespace CSE;

RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}

void RenderContainer::Register(SIRender* object, RenderGroupMode groupMode) {
    switch (groupMode) {
        case FORWARD:
            m_forwardRenderGroup->RegisterObject(object);
            break;
        case DEFERRED:
            m_deferredRenderGroup->RegisterObject(object);
            break;
        case DEPTH_ONLY:
            m_depthOnlyRenderGroup->RegisterObject(object);
            break;
    }
}

void RenderContainer::Remove(SIRender* object, RenderContainer::RenderGroupMode groupFlag) {
    switch (groupFlag) {
        case FORWARD:
            m_forwardRenderGroup->RemoveObjects(object);
            break;
        case DEFERRED:
            m_depthOnlyRenderGroup->RemoveObjects(object);
            break;
        case DEPTH_ONLY:
            m_depthOnlyRenderGroup->RegisterObject(object);
            break;
    }
}

void RenderContainer::Exterminate() {
    m_forwardRenderGroup->Exterminate();
    SAFE_DELETE(m_forwardRenderGroup);
    m_deferredRenderGroup->Exterminate();
    SAFE_DELETE(m_deferredRenderGroup);
    m_depthOnlyRenderGroup->Exterminate();
    SAFE_DELETE(m_depthOnlyRenderGroup);
}