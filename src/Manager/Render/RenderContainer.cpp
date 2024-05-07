#include "RenderContainer.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Util/Render/SGBuffer.h"
#include "../../Util/Settings.h"

using namespace CSE;

RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}

void RenderContainer::Register(SIRender* object, RenderGroupMode groupMode) {
    switch (groupMode) {
        case FORWARD: {
            const auto& group = m_forwardRenderGroup;
#ifndef CSE_SETTINGS_RENDER_FORWARD_SUPPORT
            group = m_deferredRenderGroup;
#endif
            group->RegisterObject(object);
            break;
        }
        case DEFERRED: {
            const auto& group = m_deferredRenderGroup;
#ifndef CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
            group = m_forwardRenderGroup;
#endif
            group->RegisterObject(object);
            break;
        }
        case DEPTH_ONLY:
            m_depthOnlyRenderGroup->RegisterObject(object);
            break;
#ifdef CSE_SETTINGS_RENDER_SDFGI_SUPPORT
        case SDF:
            m_sdfRenderGroup->RegisterObject(object);
            break;
#endif
    }
}

void RenderContainer::Remove(SIRender* object, RenderContainer::RenderGroupMode groupFlag) {
    switch (groupFlag) {
        case FORWARD: {
            const auto& group = m_forwardRenderGroup;
#ifndef CSE_SETTINGS_RENDER_FORWARD_SUPPORT
            group = m_deferredRenderGroup;
#endif
            group->RemoveObjects(object);
            break;
        }
        case DEFERRED: {
            const auto& group = m_deferredRenderGroup;
#ifndef CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
            group = m_forwardRenderGroup;
#endif
            group->RemoveObjects(object);
            break;
        }
        case DEPTH_ONLY:
            m_depthOnlyRenderGroup->RemoveObjects(object);
            break;
#ifdef CSE_SETTINGS_RENDER_SDFGI_SUPPORT
        case SDF:
            m_sdfRenderGroup->RemoveObjects(object);
            break;
#endif
    }
}

void RenderContainer::Exterminate() {
    if (m_forwardRenderGroup != nullptr) {
        m_forwardRenderGroup->Exterminate();
        delete m_forwardRenderGroup;
        m_forwardRenderGroup = nullptr;
    }
    if (m_deferredRenderGroup != nullptr) {
        m_deferredRenderGroup->Exterminate();
        delete m_deferredRenderGroup;
        m_deferredRenderGroup = nullptr;
    }
    if (m_depthOnlyRenderGroup != nullptr) {
        m_depthOnlyRenderGroup->Exterminate();
        delete m_depthOnlyRenderGroup;
        m_depthOnlyRenderGroup = nullptr;
    }
    if (m_sdfRenderGroup != nullptr) {
        m_sdfRenderGroup->Exterminate();
        delete m_sdfRenderGroup;
        m_sdfRenderGroup = nullptr;
    }
}