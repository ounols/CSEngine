//
// Created by ounols on 19. 5. 4.
//
#pragma once

#include "../GLProgramHandle.h"
#include "SCubeTexture.h"

class PBRShaderLoader {
public:
    PBRShaderLoader();

    ~PBRShaderLoader();

    void LoadShader();
private:
    void LoadCubeVAO(GLProgramHandle* handler);
    void RenderCubeVAO();
private:
    GLProgramHandle* m_equirectangularToCubemapShader;
    GLProgramHandle* m_irradianceShader;

    STexture* m_hdrTexture;
    SCubeTexture* m_envCubemap;
    SCubeTexture* m_irradianceMap;

    unsigned int m_cubeVBO;
    unsigned int m_cubeVAO;
};

