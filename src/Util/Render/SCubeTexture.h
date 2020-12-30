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

        bool InitTexture(int size) override;

        void Bind(GLint location, int layout) override;

        void GenerateMipmap();

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;
    };

}