#pragma once
#include <functional>
#include "../../Matrix.h"

namespace CSE {

    struct GLMeshID;

    class GLProgramHandle;

    class SMaterial;

    class VolumeTextureGenerator {
    public:
        VolumeTextureGenerator();

        ~VolumeTextureGenerator();

        void GenerateVolumeTexture(unsigned int level, const GLMeshID& mesh, const SMaterial& material);

    private:
        static int GetIndex(int level, int x, int y, int z);

        static void Render(int level, const GLMeshID& mesh, GLProgramHandle* handle);

        static unsigned char* CaptureBuffer();

        static int SavePng(const char* filename, int width, int height, int chennel, void* data);

        void ProcessAxisRender(unsigned int level, const GLMeshID& mesh, GLProgramHandle* handle,
                              unsigned char* data, const CSE::mat4& view,
                              const std::function<int(int, int, int, int)>& indexCalculator);

        void SetupFramebuffer(unsigned int& captureFBO, unsigned int& captureRBO,
                             unsigned int& tex, unsigned int tex2d_size);

    private:
        GLProgramHandle* m_handle = nullptr;
    };
}
