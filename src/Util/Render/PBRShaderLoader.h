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
    GLProgramHandle* m_equirectangularToCubemapShader;
    GLProgramHandle* m_irradianceShader;

    STexture* m_hdrTexture;
    SCubeTexture* m_envTexture;
    SCubeTexture* m_irradianceMap;
};

