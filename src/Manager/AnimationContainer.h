//
// Created by ounols on 19. 3. 31.
//

#pragma once

#include "../SObject.h"
#include "../Util/Animation/AnimationUtill.h"
#include "Base/SContainer.h"

class Animation;

class AnimationContainer : public SObject, public SContainer<Animation*> {
public:
    AnimationContainer();
    ~AnimationContainer();

    void Exterminate() override;
};


