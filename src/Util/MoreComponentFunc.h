//
// Created by ounols on 19. 8. 31.
//
#pragma once

#include "../Component/SComponent.h"

namespace CSE {

    class MoreComponentFunc {
    private:
        MoreComponentFunc() {}

        ~MoreComponentFunc() {}

    public:
        static SComponent* CreateComponent(SGameObject* obj, std::string component_type);
    };
}