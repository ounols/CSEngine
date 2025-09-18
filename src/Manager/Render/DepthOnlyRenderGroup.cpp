#include "DepthOnlyRenderGroup.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/ShaderUtil.h"
#include "../../Util/Render/SMaterial.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/RenderComponent.h"
#include "RenderMgr.h"
#include "../LightMgr.h"
#include "../../Util/GLProgramHandle.h"
#include "../../Util/AssetsDef.h"

using namespace CSE;

DepthOnlyRenderGroup::DepthOnlyRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
}

void DepthOnlyRenderGroup::RegisterObject(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    if (material == nullptr) return;

    auto& depthGroup = m_depthObjects[material->GetShaders()];
    depthGroup.push_back(object);
}

void DepthOnlyRenderGroup::RemoveObjects(SIRender* object) {
    const auto& material = object->GetMaterialReference();
    if (material == nullptr) return;

    const auto& shaders = material->GetShaders();
    if (shaders == nullptr) return;

    auto handlerPair = m_depthObjects.find(shaders);
    if (handlerPair != m_depthObjects.end()) {
        auto& layerVector = handlerPair->second;
        layerVector.erase(std::remove(layerVector.begin(), layerVector.end(), object),
                          layerVector.end());

        // 렌더링 객체가 제거된 뒤에도 공간이 비어 있는 경우, 그 공간을 제거한다.
        if (layerVector.empty()) m_depthObjects.erase(handlerPair);
    }
}

void DepthOnlyRenderGroup::RenderAll(const CameraBase& camera) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    if (frameBuffer == nullptr) {
        m_renderMgr->GetMainBuffer()->AttachFrameBuffer();
    } else {
        frameBuffer->AttachFrameBuffer();
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto& shadowObject : m_depthObjects) {
        const auto& handle = shadowObject.first->GetDepthOnlyHandle();
        if (handle == nullptr) continue;
        glUseProgram(handle->Program);
        for(const auto& render : shadowObject.second) {
            if (!render->isRenderActive) continue;
            const auto& shadowTransform = static_cast<RenderComponent*>(render)->GetGameObject()->GetTransform();

            if (LightMgr::SHADOW_DISTANCE < vec3::Distance(cameraMatrix.cameraPosition, shadowTransform->m_position))
                continue;

            render->SetMatrix(cameraMatrix, handle);
            render->Render(handle);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthOnlyRenderGroup::Exterminate() {
    m_depthObjects.clear();
}