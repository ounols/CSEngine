//
// Created by ounols on 19. 5. 4.
//
#pragma once

#include "../GLProgramHandle.h"
#include "SCubeTexture.h"

namespace CSE {

    class PBRShaderLoader {
    public:
        PBRShaderLoader();

        ~PBRShaderLoader();

        void LoadShader();

    private:
        void LoadCubeVAO();

        void RenderCubeVAO();

        void LoadPlaneVAO();

        void RenderPlaneVAO();

    private:
        GLProgramHandle* m_equirectangularToCubemapShader;
        GLProgramHandle* m_irradianceShader;
        GLProgramHandle* m_prefilterShader;
        GLProgramHandle* m_brdfShader;


        STexture* m_hdrTexture;
        SCubeTexture* m_envCubemap;
        SCubeTexture* m_irradianceMap;
        SCubeTexture* m_prefilterMap;

        unsigned int m_cubeVBO;
        unsigned int m_cubeVAO;
        unsigned int m_planeVBO;
        unsigned int m_planeVAO;
    };

}