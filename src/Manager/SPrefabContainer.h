//
// Created by ounols on 19. 3. 17.
//

#pragma once

#include "../SObject.h"
#include "Base/SContainer.h"
#include "../Object/SPrefab.h"


class SPrefabContainer : public SObject, public SContainer<SPrefab*> {
public:
    SPrefabContainer();

    ~SPrefabContainer();

    void Exterminate() override;
};


