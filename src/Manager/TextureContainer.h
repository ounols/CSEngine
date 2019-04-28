//
// Created by ounols on 19. 4. 27.
//

#pragma once
#include "../SObject.h"
#include <vector>
#include "../Util/Render/STexture.h"
#include "Base/SContainer.h"




class TextureContainer : public SObject, public SContainer<STexture*>  {
public:
    TextureContainer();

    ~TextureContainer();

    void Exterminate() override;
};


