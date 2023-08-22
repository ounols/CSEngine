//
// Created by ounol on 2021-03-22.
//
#pragma once

#include "EngineCoreInstance.h"

#define CORE EngineCore::getInstance()

namespace CSE {
    class EngineCore : public EngineCoreInstance {
    public:
        DECLARE_SINGLETON(EngineCore);
        ~EngineCore() override;
    };
}