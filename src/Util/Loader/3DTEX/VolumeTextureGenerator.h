#pragma once

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
        GLProgramHandle* m_handle = nullptr;
    };
}