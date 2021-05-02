//
// Created by ounols on 19. 5. 4.
//

#pragma once

#include "STexture.h"

namespace CSE {

    class SCubeTexture : public STexture {
    public:
        SCubeTexture();

        ~SCubeTexture();

        bool InitTexture(int width, int height, int channel = GL_RGB, int internalFormat = GL_RGB,
                         int glType = GL_UNSIGNED_BYTE) override;
        bool InitTextureMipmap(int width, int height, int channel = GL_RGB, int internalFormat = GL_RGB,
                               int glType = GL_UNSIGNED_BYTE);

        void Bind(GLint location, int layout) override;

        void GenerateMipmap();

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;
    };

}