//
// Created by ounols on 19. 8. 31.
//
#pragma once

#include "../Component/SComponent.h"
#include "sqext.h"

namespace CSE {

    class MoreComponentFunc {
    private:
        MoreComponentFunc() {}

        ~MoreComponentFunc() {}

    public:
        static void BindComponentToSQInstance(SComponent* component, std::string name,
                                              sqext::SQIClassInstance* instance);
    };
}