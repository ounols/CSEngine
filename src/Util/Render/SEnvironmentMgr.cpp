//
// Created by ounols on 19. 5. 4.
//
#include "SEnvironmentMgr.h"
#include "ShaderUtil.h"
#include "../AssetsDef.h"
#include "../Matrix.h"
#include "SkyboxUtil.h"
#include "../CaptureDef.h"

#include <string>
#include <iostream>
#include "../Loader/STB/stb_image.h"
#include "SMaterial.h"
#include "../Settings.h"
#include "../GLProgramHandle.h"

using namespace CSE;

unsigned int SEnvironmentMgr::m_width = 0;
unsigned int SEnvironmentMgr::m_height = 0;
unsigned int SEnvironmentMgr::m_planeVAO = 0;
unsigned int SEnvironmentMgr::m_cubeVAO = 0;

SEnvironmentMgr::SEnvironmentMgr() {

}

SEnvironmentMgr::~SEnvironmentMgr() {
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
}

void SEnvironmentMgr::RenderPBREnvironment() {

    std::string cubemap_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/cubemap.vert");
    std::string etc_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/equirectangular_to_cubemap.frag");
    std::string irr_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/irradiance_convolution.frag");
    std::string pre_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/prefilter.frag");
    std::string brdf_v_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/brdf.vert");
    std::string brdf_f_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/PBR/IBL/brdf.frag");

    m_equirectangularToCubemapShader = ShaderUtil::CreateProgramHandle(cubemap_str.c_str(), etc_str.c_str());
    m_irradianceShader = ShaderUtil::CreateProgramHandle(cubemap_str.c_str(), irr_str.c_str());
    m_prefilterShader = ShaderUtil::CreateProgramHandle(cubemap_str.c_str(), pre_str.c_str());
    m_brdfShader = ShaderUtil::CreateProgramHandle(brdf_v_str.c_str(), brdf_f_str.c_str());


    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;

    glDisable(GL_CULL_FACE);

    glGenRenderbuffers(1, &captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 512, 512);

    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    std::string hdr_path = CSE::AssetsPath() + "Texture/hdr/newport_loft.hdr";
    m_hdrTexture = SResource::Create<STexture>(hdr_path);
    stbi_set_flip_vertically_on_load(false);

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    m_envCubemap = new STexture(STexture::TEX_CUBE);
    m_envCubemap->SetName("envCubemap.textureCubeMap");
    m_envCubemap->SetID("envCubemap.textureCubeMap");
    m_envCubemap->InitTextureMipmap(512, 512);


    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    mat4 captureProjection = mat4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
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
    // ----------------------------------------------------------------------
    glUseProgram(m_equirectangularToCubemapShader->Program);
    int hdrTextureLocation = m_equirectangularToCubemapShader->UniformLocation("EquirectangularMap")->id;
    m_equirectangularToCubemapShader->SetUniformMat4("PROJECTION_MATRIX", captureProjection);

    m_hdrTexture->Bind(hdrTextureLocation, 0);

    LoadCubeVAO();
    glGetError();
    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    std::cout << "[PBR] Generating Environment Cubemap...";
    for (unsigned int i = 0; i < 6; ++i) {
        m_equirectangularToCubemapShader->SetUniformMat4("VIEW_MATRIX", captureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_envCubemap->GetTextureID(), 0);
        glGetError();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderCubeVAO();
//        std::string save_str = CSE::AssetsPath() + "test" + std::to_string(i) + ".bmp";
//        saveScreenshot(save_str.c_str());
    }
    m_envCubemap->GenerateMipmap();

    std::cout << " finished!\n";

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    m_irradianceMap = new STexture(STexture::TEX_CUBE);
    m_irradianceMap->SetName("irradiance.textureCubeMap");
    m_irradianceMap->SetID("irradiance.textureCubeMap");
    m_irradianceMap->InitTexture(32, 32);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    glUseProgram(m_irradianceShader->Program);
	int envTextureLocation = m_irradianceShader->UniformLocation("EnvironmentMap")->id;
    m_irradianceShader->SetUniformMat4("PROJECTION_MATRIX", captureProjection);
    m_envCubemap->Bind(envTextureLocation, 0);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    std::cout << "[PBR] Generating Irradiance Cubemap...";
    for (unsigned int i = 0; i < 6; ++i) {
        m_irradianceShader->SetUniformMat4("VIEW_MATRIX", captureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_irradianceMap->GetTextureID(), 0);
        glGetError();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderCubeVAO();
    }

    std::cout << " finished!\n";

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    m_prefilterMap = new STexture(STexture::TEX_CUBE);
    m_prefilterMap->SetName("prefilter.textureCubeMap");
    m_prefilterMap->SetID("prefilter.textureCubeMap");
    m_prefilterMap->InitTextureMipmap(128, 128);

    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    m_prefilterMap->GenerateMipmap();

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    glUseProgram(m_prefilterShader->Program);
    envTextureLocation = m_prefilterShader->UniformLocation("EnvironmentMap")->id;
    m_prefilterShader->SetUniformMat4("PROJECTION_MATRIX", captureProjection);
    m_envCubemap->Bind(envTextureLocation, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;

    std::cout << "[PBR] Backing Prefiltering Textures...";
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float) mip / (float) (maxMipLevels - 1);
        m_prefilterShader->SetUniformFloat("FLOAT_ROUGHNESS", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            m_prefilterShader->SetUniformMat4("VIEW_MATRIX", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   m_prefilterMap->GetTextureID(), mip);

            glGetError();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCubeVAO();
//            std::string save_str = CSE::AssetsPath() + "mip_" + std::to_string(mip) + "_" + std::to_string(i) + ".bmp";
//            saveScreenshot(save_str.c_str());
        }
        std::cout << "Level " << mip << "..";
    }
    std::cout << " finished!\n";

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    std::cout << "[PBR] Backing a 2D LUT from the BRDF equations used...";
    m_brdfMap = new STexture();
    m_brdfMap->SetName("brdfLUT.texture");
    m_brdfMap->SetID("brdfLUT.texture");
    m_brdfMap->InitTexture(512, 512, GL_RG, GL_RG16F, GL_FLOAT);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfMap->GetTextureID(), 0);

    glViewport(0, 0, 512, 512);
    glUseProgram(m_brdfShader->Program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LoadPlaneVAO();
    RenderPlaneVAO();

//    std::string save_str = CSE::AssetsPath() + "brdf.bmp";
//    saveScreenshot(save_str.c_str());
    std::cout << " finished!\n";

    //release fbo, rbo
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
}


void SEnvironmentMgr::InitShadowEnvironment() {
    std::string vert_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/Shadow/default_shadow.vert");
    std::string frag_str = CSE::AssetMgr::LoadAssetFile(CSE::AssetsPath() + "Shader/Shadow/default_shadow.frag");

    m_shadowDepthMapShader = ShaderUtil::CreateProgramHandle(vert_str.c_str(), frag_str.c_str());
}


void SEnvironmentMgr::LoadCubeVAO() {
    float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
    };
    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(m_cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SEnvironmentMgr::RenderCubeVAO() {
    // render Cube
    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void SEnvironmentMgr::LoadPlaneVAO() {
    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &m_planeVAO);
    glGenBuffers(1, &m_planeVBO);
    glBindVertexArray(m_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
}

void SEnvironmentMgr::RenderPlaneVAO() {
    glBindVertexArray(m_planeVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int SEnvironmentMgr::GetWidth() {
    return m_width;
}

void SEnvironmentMgr::SetWidth(unsigned int width) {
    m_width = width;
}

unsigned int SEnvironmentMgr::GetHeight() {
    return m_height;
}

void SEnvironmentMgr::SetHeight(unsigned int height) {
    m_height = height;
}

unsigned int* SEnvironmentMgr::GetPointerWidth() {
    return &m_width;
}

unsigned int* SEnvironmentMgr::GetPointerHeight() {
    return &m_height;
}

void SEnvironmentMgr::BindPBREnvironmentMap(const GLProgramHandle* handle, int textureLayout) const {
    if(handle == nullptr) return;
    m_irradianceMap->Bind(handle->Uniforms.LightIrradiance, textureLayout);
    m_prefilterMap->Bind(handle->Uniforms.LightPrefilter, textureLayout + 1);
    m_brdfMap->Bind(handle->Uniforms.LightBrdfLut, textureLayout + 2);
}
