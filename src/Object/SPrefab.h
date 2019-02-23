//
// Created by ounols on 19. 2. 10.
//

#pragma once
#include "../SObject.h"
#include "SGameObject.h"


class SPrefab : public SObject {
public:
    SPrefab();
    ~SPrefab();

    void Exterminate() override;

private:
    SGameObject* m_root = nullptr;
};


