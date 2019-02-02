//
// Created by ounols on 19. 1. 22.
//

#pragma once

#include "../SObject.h"

#include <vector>
#include "Base/SContainer.h"
//#include "../Util/Render/Skeleton.h"

class Skeleton;

class SkeletonContainer : public SObject, public SContainer<Skeleton*> {
public:
    SkeletonContainer();
    ~SkeletonContainer();

    void Exterminate() override;
};


