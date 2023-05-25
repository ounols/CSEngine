#include "VolumeTextureGenerator.h"
#include "../../../OGLDef.h"
#include "../../GLProgramHandle.h"
#include "../../Render/GLMeshID.h"
#include "../../Render/SMaterial.h"
#include "../../Render/ShaderUtil.h"
#include "../../Render/SEnvironmentMgr.h"
#include "../../Render/STexture.h"
#include "../../CaptureDef.h"
#include "../../AssetsDef.h"

CSE::VolumeTextureGenerator::VolumeTextureGenerator() {
    m_handle = SResource::Create<GLProgramHandle>("File:Shader/SDF/ModelGenerator.shader");
}

CSE::VolumeTextureGenerator::~VolumeTextureGenerator() {
}

void
CSE::VolumeTextureGenerator::GenerateVolumeTexture(unsigned int level, const GLMeshID& mesh, const SMaterial& material) {

    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int tex;
    int size = level * level;
    unsigned int tex2d_size = size * level;
    const auto& max_distance = mesh.m_maxSize + mesh.m_maxSize / 2.f;
    const auto half_distance = max_distance / 2.f;

    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenRenderbuffers(1, &captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, tex2d_size, tex2d_size);

    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex2d_size, tex2d_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, tex2d_size, tex2d_size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    const auto& handle = m_handle;
    mat4 identity = mat4::Identity();
    mat4 view = mat4::RotateZ(180.f);

    glUseProgram(handle->Program);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    material.AttachElement();
    {
        auto albedo_tex = handle->UniformLocation("texture.albedo");
        if(albedo_tex != nullptr && albedo_tex->id >= 0) {
            const auto& e = material.GetElement("texture.albedo");
            if(e != nullptr) {
                STexture* tex = SResource::Create<STexture>(e->raw);
//              tex->Bind(albedo_tex->id, 0);
            }
        }

        if(handle->Uniforms.ViewMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ViewMatrix, 1, 0, view.Pointer());
    }

    for(unsigned int i = 0; i < size; ++i) {
        float cell = max_distance / size;
        glViewport((i % level) * size, (i / level) * size, size, size);
        mat4 projection = mat4::Ortho(-max_distance, max_distance,
                                      -max_distance, max_distance,
                                      -half_distance + i * cell, -half_distance + (i + 1.f) * cell);
//                                          -100, 100);
        if(handle->Uniforms.ProjectionMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ProjectionMatrix, 1, 0, projection.Pointer());

        ShaderUtil::BindAttributeToShader(*handle, mesh);

    }

    std::string save_str = CSE::AssetsPath() + "test" + ".png";
    glViewport(0, 0, tex2d_size, tex2d_size);
    saveScreenshot(save_str.c_str());
    glEnable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}