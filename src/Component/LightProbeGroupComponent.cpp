#include "LightProbeGroupComponent.h"
#include "../Util/Render/LightProbe/LightProbeData.h"
#include "../Util/Render/LightProbe/SProbeTexture.h"
#include "../OGLDef.h"


using namespace CSE;

COMPONENT_CONSTRUCTOR(LightProbeGroupComponent) {

}

LightProbeGroupComponent::~LightProbeGroupComponent() = default;

void LightProbeGroupComponent::InitProbeTexture(Vector3<unsigned short> m_size) {

    const unsigned short width = m_size.x * PER_WIDTH * 2;
    const unsigned short height = m_size.y * m_size.z * PER_WIDTH;

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const auto& transform = gameObject->GetTransform();
    m_pivot = &transform->m_position;
    m_scale = &transform->m_scale;
    m_nodeOffset = vec3{ m_scale->x / m_size.x, m_scale->y / m_size.y, m_scale->z / m_size.z };
}

void LightProbeGroupComponent::Render(unsigned int framebufferId) const {

    // TODO: 각 라이트 프로브의 노드들을 렌더링하기 시작함.

    for(unsigned short x = 0; x < m_size.x; ++x) {
        const unsigned short u_offset = x * PER_WIDTH * 2;
        const float x_offset = x * m_nodeOffset.x;
        for(unsigned short y = 0; y < m_size.y; ++y) {
            const unsigned short h_offset = y * m_size.y * PER_WIDTH;
            const float y_offset = y * m_nodeOffset.y;
            for(unsigned short z = 0; z < m_size.z; ++z) {
                const unsigned short v_offset = h_offset + z * PER_WIDTH;
                const float z_offset = z * m_nodeOffset.z;

                glViewport(u_offset, v_offset, PER_WIDTH, PER_WIDTH);
            }
        }
    }

//    for (const auto& node : m_nodes) {
//        auto texId = node->m_texture->GetTextureID();
//        for(unsigned short i = 0; i < 6; ++i) {
//            // TODO: 카메라 매트릭스를 넣어야 함
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                                   texId, 0);
//            glClear(GL_COLOR_BUFFER_BIT);
//            // TODO: RenderCubeVAO();
//        }
//    }
}
