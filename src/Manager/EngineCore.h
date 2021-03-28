//
// Created by ounol on 2021-03-22.
//
#pragma once

#include <vector>
#include <queue>
#include <list>
#include "../MacroDef.h"
#include "Base/CoreBase.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "OGLMgr.h"
#include "ScriptMgr.h"
#include "CameraMgr.h"
#include "LightMgr.h"
#include "RenderMgr.h"
#include "SceneMgr.h"

#define GET_CORE_FUNCTION(CORENAME, core_instance)  \
CORENAME* Get##CORENAME##Core() const {               \
    return static_cast<CORENAME*>(core_instance);   \
}

#define CORE EngineCore::getInstance()
#define GetCore(CORENAME) Get##CORENAME##Core()

namespace CSE {
    class EngineCore {
    public:
        DECLARE_SINGLETONE(EngineCore);
        ~EngineCore();

        GET_CORE_FUNCTION(ResMgr, m_resMgr);
        GET_CORE_FUNCTION(GameObjectMgr, m_gameObjectMgr);
        GET_CORE_FUNCTION(RenderMgr, m_renderMgr);
        GET_CORE_FUNCTION(CameraMgr, m_cameraMgr);
        GET_CORE_FUNCTION(LightMgr, m_lightMgr);
        GET_CORE_FUNCTION(SceneMgr, m_sceneMgr);
        GET_CORE_FUNCTION(MemoryMgr, m_memoryMgr);
        GET_CORE_FUNCTION(ScriptMgr, m_scriptMgr);

        void Init(unsigned int width, unsigned int height);
        void Update(float elapsedTime);
        void Render() const;
        void Exterminate();
        void ResizeWindow(unsigned int width, unsigned int height);

    private:
        void GenerateCores();

    private:
        std::vector<CoreBase*> m_cores;
        std::list<RenderCoreBase*> m_renderCores;
        std::list<CoreBase*> m_updateCores;

        //For Reference
        CoreBase* m_resMgr = nullptr;
        CoreBase* m_gameObjectMgr = nullptr;
        CoreBase* m_renderMgr = nullptr;
        CoreBase* m_cameraMgr = nullptr;
        CoreBase* m_lightMgr = nullptr;
        CoreBase* m_sceneMgr = nullptr;
        CoreBase* m_memoryMgr = nullptr;
        CoreBase* m_scriptMgr = nullptr;

        OGLMgr* m_oglMgr = nullptr;
    };
}