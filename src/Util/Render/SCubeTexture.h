//
// Created by ounols on 19. 5. 4.
//

#pragma once

#include "STexture.h"


class SCubeTexture : public STexture {
public:
    SCubeTexture();

    ~SCubeTexture();

    bool InitTexture(int size) override;

    void Bind(GLint location, int layout) override;
    void GenerateMipmap();

};



