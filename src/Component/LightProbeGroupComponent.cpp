#include "LightProbeGroupComponent.h"
#include "../Util/Render/LightProbe/LightProbeData.h"
#include "../Util/Render/LightProbe/SProbeTexture.h"
#include "../Util/Render/STexture.h"


using namespace CSE;

COMPONENT_CONSTRUCTOR(LightProbeGroupComponent) {

}

LightProbeGroupComponent::~LightProbeGroupComponent() = default;

void LightProbeGroupComponent::Render(unsigned int framebufferId) const {

    // TODO: 각 라이트 프로브의 노드들을 렌더링하기 시작함.
    // 거리에 따라 적용을 할까...?

    for (const auto& node : m_nodes) {
        auto texId = node->m_texture->GetTextureID();
        for(unsigned short i = 0; i < 6; ++i) {
            // TODO: 카메라 매트릭스를 넣어야 함
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   texId, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            // TODO: RenderCubeVAO();
        }
    }
}

LightProbeResultData LightProbeGroupComponent::GetLightProbe(const vec3& position) const {

    // TODO: 가장 가까운 거리의 LightProbeData 3개의 필수 데이터들만 전달함.

    return LightProbeResultData{{ 1, 2,  3 },
                                { 1, 23, 3 }};
}