//
// Created by ounol on 2021-03-22.
//
#pragma once

#include "EngineCoreInstance.h"

#if defined(__CSE_EDITOR__)
#include "../../Editor/src/Manager/EEngineCore.h"
#else
#define CORE EngineCore::getInstance()
#endif

namespace CSE {
    class EngineCore : public EngineCoreInstance {
    public:
        DECLARE_SINGLETON(EngineCore);
        ~EngineCore() override;
    };
}