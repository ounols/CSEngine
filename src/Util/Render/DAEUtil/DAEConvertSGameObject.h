//
// Created by ounols on 19. 2. 9.
//

#pragma once

#include "../../../Object/SGameObject.h"


class DAEConvertSGameObject {
public:
    DAEConvertSGameObject();
    ~DAEConvertSGameObject();

    static SGameObject* GetJoints(SGameObject* parent, Joint* data);
};