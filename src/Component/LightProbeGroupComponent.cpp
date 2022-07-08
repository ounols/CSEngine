#include "LightProbeGroupComponent.h"
#include "../OGLDef.h"
#include "../Util/Render/MeshSurface.h"
#include "../Util/Render/SMaterial.h"
#include "CameraComponent.h"
#include "../Manager/LightMgr.h"
#include "../Manager/EngineCore.h"


using namespace CSE;

COMPONENT_CONSTRUCTOR(LightProbeGroupComponent) {
    m_lightMgr = CORE->GetCore(LightMgr);
    m_lightMgr->RegisterLightProbeGroup(this);
}

LightProbeGroupComponent::~LightProbeGroupComponent() {
    SAFE_DELETE(m_cameraMatrixStruct);
    for (auto* node : m_nodes) {
        SAFE_DELETE(node);
    }
}

void LightProbeGroupComponent::InitProbeTexture(Vector3<unsigned short> m_nodeCount) {

    const unsigned short width = m_nodeCount.x * PER_WIDTH * 2;
    const unsigned short height = m_nodeCount.y * m_nodeCount.z * PER_WIDTH;

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
    m_nodeOffset = vec3{ m_scale->x / m_nodeCount.x, m_scale->y / m_nodeCount.y, m_scale->z / m_nodeCount.z };
    m_size = m_nodeCount;

    m_cameraMatrixStruct = new CameraMatrixStruct(mat4::Identity(), mat4::Perspective(160.0f, 1.0f, 0.01f, 10.0f),
                                                  vec3{0, 0, 0});
    m_frontCamera = mat4::LookAt(vec3{0, 0, 0}, vec3{1, 0, 0}, vec3{0, 1, 0});
    m_backCamera = mat4::LookAt(vec3{0, 0, 0}, vec3{-1, 0, 0}, vec3{0, 1, 0});

    GenerateCameraNodeList();
}

void LightProbeGroupComponent::RenderAll(unsigned int framebufferId, const std::list<SIRender*>& renderList) const {

    // TODO: 각 라이트 프로브의 노드들을 렌더링하기 시작함.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    const unsigned int size = m_nodes.size();

    for (int i = m_renderOffset; i < size; i += m_renderSnap) {
        const auto& node = m_nodes[i];
        const auto& uv = node->uv;
        const auto& position = node->position;
        const vec3 result_position = position + *m_pivot;
        m_cameraMatrixStruct->cameraPosition = result_position;
        m_cameraMatrixStruct->camera = mat4::LookAt(result_position, result_position + vec3{1, 0, 0}, vec3{0, 1, 0});

        glViewport(uv.x, uv.y, PER_WIDTH, PER_WIDTH);
        RenderInstances(renderList);
    }
    ++m_renderOffset;
    if(m_renderOffset >= m_renderSnap) m_renderOffset = 0;

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

void LightProbeGroupComponent::RenderInstances(const std::list<SIRender*>& renderList) const {

    for (const auto& render : renderList) {
        // 길이 측정을 넣고싶다...
        const auto& handle = render->GetMaterial()->GetHandle();
        glUseProgram(handle->Program);
        m_lightMgr->AttachLightToShader(handle);
        render->SetMatrix(*m_cameraMatrixStruct);
        render->Render();
    }

}

void LightProbeGroupComponent::GenerateCameraNodeList() {
    unsigned int size = m_size.x * m_size.y * m_size.z;
    m_nodes.reserve(size);
    m_renderSnap = size / 60.f;

    for(unsigned short x = 0; x < m_size.x; ++x) {
        const unsigned short u_offset = x * PER_WIDTH * 2;
        const float x_offset = x * m_nodeOffset.x;

        for (unsigned short y = 0; y < m_size.y; ++y) {
            const unsigned short h_offset = y * m_size.y * PER_WIDTH;
            const float y_offset = y * m_nodeOffset.y;

            for (unsigned short z = 0; z < m_size.z; ++z) {
                const unsigned short v_offset = h_offset + z * PER_WIDTH;
                const float z_offset = z * m_nodeOffset.z;

                auto* node = new LightProbeCameraNode();
                node->position = vec3{ x_offset, y_offset, z_offset };
                node->uv = Vector2<unsigned short>{ v_offset, u_offset };
                m_nodes.push_back(node);
            }
        }
    }
}

void LightProbeGroupComponent::Init() {

}

void LightProbeGroupComponent::Tick(float elapsedTime) {

}

void LightProbeGroupComponent::Exterminate() {

}
