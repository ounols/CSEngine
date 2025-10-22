//
// Created by ounols on 19. 8. 31.
//
#pragma once
#ifndef CSE_GLOBAL_SCRIPT_DISABLED
#include "../Component/SComponent.h"
#include "sqext.h"

namespace CSE {

    class MoreComponentFunc {
    private:
        MoreComponentFunc() {}

        ~MoreComponentFunc() = default;

    public:
        static void BindComponentToSQInstance(SComponent* component, const std::string& name,
                                              sqext::SQIClassInstance* instance);
    };
}
#endif