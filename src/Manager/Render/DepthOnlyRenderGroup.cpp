#include "DepthOnlyRenderGroup.h"
#include "../../Util/Render/CameraBase.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/ShaderUtil.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/RenderComponent.h"
#include "RenderMgr.h"
#include "../LightMgr.h"
#include "../../Util/GLProgramHandle.h"
#include "../../Util/AssetsDef.h"

using namespace CSE;

DepthOnlyRenderGroup::DepthOnlyRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
    std::string vert_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/Shadow/default_shadow.vert");
    std::string frag_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/Shadow/default_shadow.frag");

    m_shadowHandle = ShaderUtil::CreateProgramHandle(vert_str.c_str(), frag_str.c_str());
}

void DepthOnlyRenderGroup::RegisterObject(SIRender* object) {
    m_depthObjects.push_back(object);
}

void DepthOnlyRenderGroup::RemoveObjects(SIRender* object) {
    m_depthObjects.remove(object);
}

void DepthOnlyRenderGroup::RenderAll(const CameraBase& camera) const {
    const auto cameraMatrix = camera.GetCameraMatrixStruct();
    const auto& frameBuffer = camera.GetFrameBuffer();
    const unsigned int program = m_shadowHandle->Program;
    if(frameBuffer == nullptr) {
        m_renderMgr->GetMainBuffer()->AttachFrameBuffer();
    }
    else {
        frameBuffer->AttachFrameBuffer();
    }

    glUseProgram(program);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto& shadowObject : m_depthObjects) {
        if(!shadowObject->isRenderActive) continue;
        const auto& shadowTransform = static_cast<RenderComponent*>(shadowObject)->GetGameObject()->GetTransform();

        if(LightMgr::SHADOW_DISTANCE < vec3::Distance(cameraMatrix.cameraPosition, shadowTransform->m_position))
            continue;

        shadowObject->SetMatrix(cameraMatrix, m_shadowHandle);
        shadowObject->Render(m_shadowHandle);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthOnlyRenderGroup::Exterminate() {
    m_depthObjects.clear();
}