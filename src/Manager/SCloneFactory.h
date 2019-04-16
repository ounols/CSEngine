//
// Created by ounols on 19. 2. 13.
//

#pragma once

#include "../Component/SComponent.h"


class SCloneFactory {
private:
    SCloneFactory();
    ~SCloneFactory();

public:
    static SComponent* Clone(SComponent* component, SGameObject* parent = nullptr);
    static SGameObject* Clone(SGameObject* object, SGameObject* parent = nullptr);
};


