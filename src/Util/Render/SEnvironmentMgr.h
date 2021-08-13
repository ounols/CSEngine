//
// Created by ounols on 19. 5. 4.
//
#pragma once

#include "../GLProgramHandle.h"
#include "STexture.h"

namespace CSE {

    class SEnvironmentMgr {
    public:
        SEnvironmentMgr();

        ~SEnvironmentMgr();

        void RenderPBREnvironment();

        void InitShadowEnvironment();
        GLProgramHandle* GetShadowEnvironment() const {
            return m_shadowDepthMapShader;
        }

        static unsigned int GetWidth();
        static unsigned int* GetPointerWidth();

        static void SetWidth(unsigned int width);

        static unsigned int GetHeight();
        static unsigned int* GetPointerHeight();

        static void SetHeight(unsigned int height);

    private:
        void LoadCubeVAO();

        void RenderCubeVAO();

        void LoadPlaneVAO();

        void RenderPlaneVAO();

    private:
        static unsigned int m_width;
        static unsigned int m_height;

        GLProgramHandle* m_equirectangularToCubemapShader;
        GLProgramHandle* m_irradianceShader;
        GLProgramHandle* m_prefilterShader;
        GLProgramHandle* m_brdfShader;

        GLProgramHandle* m_shadowDepthMapShader;


        STexture* m_hdrTexture;
        STexture* m_envCubemap;
        STexture* m_irradianceMap;
        STexture* m_prefilterMap;
        STexture* m_brdfMap;

        unsigned int m_cubeVBO;
        unsigned int m_cubeVAO;
        unsigned int m_planeVBO;
        unsigned int m_planeVAO;
    };
}