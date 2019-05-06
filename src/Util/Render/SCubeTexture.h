//
// Created by ounols on 19. 5. 4.
//

#pragma once

#include "STexture.h"


class SCubeTexture : public STexture {
public:
    SCubeTexture();

    ~SCubeTexture();

    bool CreateCubeTexture(int size = 512);

    void Bind(const GLProgramHandle* handle, int layout = 0) override;
    void GenerateMipmap();

};



