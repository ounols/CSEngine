//
// Created by ounols on 19. 5. 4.
//
#include "PBRShaderLoader.h"
#include "ShaderUtil.h"
#include "STexture.h"
#include "../AssetsDef.h"

#include <string>
#include "../../Manager/TextureContainer.h"
#include "../Matrix.h"
#include "../Loader/STB/stb_image_write.h"


PBRShaderLoader::PBRShaderLoader() {

}

PBRShaderLoader::~PBRShaderLoader() {

}
#include "../CaptureDef.h"
#include "../AssetsDef.h"
void PBRShaderLoader::LoadShader() {

    std::string vs_cubeMap = CSE::OpenAssetsTxtFile(CSE::AssetsPath() + "Shader/PBR/IBL/cubemap.vert");
    std::string fs_equirectangularToCubemap =
            CSE::OpenAssetsTxtFile(CSE::AssetsPath() + "Shader/PBR/IBL/equirectangular_to_cubemap.frag");
    std::string fs_irradianceConvolution =
            CSE::OpenAssetsTxtFile(CSE::AssetsPath() + "Shader/PBR/IBL/irradiance_convolution.frag");

    m_equirectangularToCubemapShader = ShaderUtil::CreateProgramHandle(vs_cubeMap.c_str(),
                                                                       fs_equirectangularToCubemap.c_str());
    m_irradianceShader = ShaderUtil::CreateProgramHandle(vs_cubeMap.c_str(),
                                                         fs_irradianceConvolution.c_str());

    GLuint captureFBO;
    GLuint captureRBO;

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    m_hdrTexture = new STexture();
    std::string hdr_path = CSE::AssetsPath() + "Texture/hdr/newport_loft.hdr";
    m_hdrTexture->LoadFile(hdr_path.c_str());


    // pbr: setup cubemap to render to and attach to framebuffer
    m_envTexture = new SCubeTexture();
    m_envTexture->CreateCubeTexture(512);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    mat4 captureProjection = mat4::Perspective(90.f, 1.0f, 0.1f, 10.0f);
    mat4 captureViews[] =
            {
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
                    mat4::LookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f))
            };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    glUseProgram(m_equirectangularToCubemapShader->Program);
    auto equirectangularMap = m_equirectangularToCubemapShader->UniformLocation("EquirectangularMap");

    m_equirectangularToCubemapShader->SetUniformInt("EquirectangularMap", 0);
    m_equirectangularToCubemapShader->SetUniformMat4("PROJECTION_MATRIX", captureProjection);

    m_hdrTexture->Bind(equirectangularMap, 0);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        m_equirectangularToCubemapShader->SetUniformMat4("VIEW_MATRIX", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_envTexture->GetID(), 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
//    m_envTexture->Bind(m_equirectangularToCubemapShader);
//    m_envTexture->GenerateMipmap();

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    m_irradianceMap = new SCubeTexture();
    m_irradianceMap->CreateCubeTexture(32);
    int index = ResMgr::getInstance()->GetID<TextureContainer, STexture>(m_envTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 32, 32);


    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    glUseProgram(m_irradianceShader->Program);
    auto environmentMap = m_irradianceShader->UniformLocation("EnvironmentMap");
    m_irradianceShader->SetUniformInt("EnvironmentMap", 0);
    m_irradianceShader->SetUniformMat4("PROJECTION_MATRIX", captureProjection);
    m_envTexture->Bind(environmentMap, 0);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        m_irradianceShader->SetUniformMat4("VIEW_MATRIX", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_irradianceMap->GetID(), 0);

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

