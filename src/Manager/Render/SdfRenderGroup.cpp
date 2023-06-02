#include "SdfRenderGroup.h"
#include "../../OGLDef.h"
#include "../../Util/Render/STexture.h"
#include "../../Util/GLProgramHandle.h"
#include "../../Util/Render/SFrameBuffer.h"
#include "../../Util/Render/ShaderUtil.h"
#include "../../Component/CameraComponent.h"

using namespace CSE;

SdfRenderGroup::SdfRenderGroup(const RenderMgr& renderMgr) : SRenderGroup(renderMgr) {
    m_envSize = 16;
    m_nodeSize = vec3{ 8, 8, 8 };
    m_nodeSpace = 0.3;

    m_sdfMapBuffer = new SFrameBuffer();
    m_sdfMapBuffer->SetName("SDF Render Group Texture");
    m_mapSize = vec2{ m_envSize * 6.f, m_nodeSize.x * m_nodeSize.y * m_nodeSize.z * m_envSize };
//    m_mapSize = ivec2{ 256, 256 };
    m_sdfMapBuffer->GenerateFramebuffer(SFrameBuffer::PLANE, m_mapSize.x, m_mapSize.y);

    m_mapTexture = m_sdfMapBuffer->GenerateTexturebuffer(SFrameBuffer::RENDER, GL_RGBA);
    m_sdfMapBuffer->GenerateRenderbuffer(SFrameBuffer::DEPTH, GL_DEPTH_COMPONENT);
    m_sdfMapBuffer->RasterizeFramebuffer();

    m_mapTexture->SetAbsoluteID("sdfmap.textureFakeCubeMap");
    std::string hash = "CSEENV0000000005";
    m_mapTexture->SetHash(hash);
    m_mapTexture->GenerateMipmap();

    // Codes for the SDF test
    m_testTexture = SResource::Create<STexture>("File:test.png");
    m_testTexture->GenerateMipmap();
    m_sdfHandle = SResource::Create<GLProgramHandle>("File:Shader/SDF/sdfmap.post");
    {
        const auto& e = m_sdfHandle->UniformLocation("sdf.tex");
        if (e != nullptr) m_testTextureId = m_sdfHandle->UniformLocation("sdf.tex")->id;
    }
    {
        const auto& e = m_sdfHandle->UniformLocation("sdf.env.size");
        if (e != nullptr) m_envSizeId = m_sdfHandle->UniformLocation("sdf.env.size")->id;
    }
    {
        const auto& e = m_sdfHandle->UniformLocation("sdf.node.size");
        if (e != nullptr) m_nodeSizeId = m_sdfHandle->UniformLocation("sdf.node.size")->id;
    }
    {
        const auto& e = m_sdfHandle->UniformLocation("sdf.node.space");
        if (e != nullptr) m_nodeSpaceId = m_sdfHandle->UniformLocation("sdf.node.space")->id;
    }
}

SdfRenderGroup::~SdfRenderGroup() = default;

void SdfRenderGroup::RegisterObject(SIRender* object) {

}

void SdfRenderGroup::RemoveObjects(SIRender* object) {

}

#include "../../Util/CaptureDef.h"
#include "../../Util/AssetsDef.h"

void SdfRenderGroup::RenderAll(const CameraBase& camera) const {
    const auto& uniforms = m_sdfHandle->Uniforms;
    const auto& cameraStruct = camera.GetCameraMatrixStruct();

    m_sdfMapBuffer->AttachFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (int)m_mapSize.x, (int)m_mapSize.y);
    glUseProgram(m_sdfHandle->Program);
    m_testTexture->Bind(m_testTextureId, 0);
    ShaderUtil::BindCameraToShader(*m_sdfHandle, cameraStruct.camera, cameraStruct.cameraPosition,
                                   cameraStruct.projection, mat4::Identity());
    glUniform2fv(uniforms.SourceBufferSize, 1, m_mapSize.Pointer());
    BindShaderUniforms(*m_sdfHandle);

    ShaderUtil::BindAttributeToPlane();

//    std::string save_str = CSE::AssetsPath() + "testmap" + ".png";
//    saveScreenshot(save_str.c_str());
}

void SdfRenderGroup::Exterminate() {

}

void SdfRenderGroup::BindShaderUniforms(const GLProgramHandle& handle) const {
    const auto& uniforms = handle.Uniforms;

    if(uniforms.SdfEnvsize != HANDLE_NULL)
        glUniform1i(uniforms.SdfEnvsize, m_envSize);
    if(uniforms.SdfNodeSize != HANDLE_NULL)
        glUniform3fv(uniforms.SdfNodeSize, 1, m_nodeSize.Pointer());
    if(uniforms.SdfNodeSpace != HANDLE_NULL)
        glUniform1f(uniforms.SdfNodeSpace, m_nodeSpace);
}
