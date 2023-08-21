//
// Created by ounol on 2021-03-22.
//
#pragma once

#include <vector>
#include <list>
#include "../MacroDef.h"
#include "Base/CoreBase.h"
#include "Base/RenderCoreBase.h"

#define GET_CORE_FUNCTION(CORENAME, core_instance)  \
CORENAME* Get##CORENAME##Core() const {               \
    return static_cast<CORENAME*>(core_instance);   \
}

#define CORE EngineCore::getInstance()
#define GetCore(CORENAME) Get##CORENAME##Core()

// Core Class Predeclaration
namespace CSE {
    class ResMgr;
    class GameObjectMgr;
    class RenderMgr;
    class CameraMgr;
    class LightMgr;
    class SceneMgr;
    class MemoryMgr;
    class ScriptMgr;
    class OGLMgr;
}

namespace CSE {
    class EngineCore {
    public:
        DECLARE_SINGLETON(EngineCore);
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
        void LateUpdate(float elapsedTime);
        void Render() const;
        void Exterminate();
        void ResizeWindow(unsigned int width, unsigned int height);
        void SetDeviceBuffer(unsigned int id);
        void GenerateCores();

    private:
        std::vector<CoreBase*> m_cores;
        std::list<RenderCoreBase*> m_renderCores;
        std::list<CoreBase*> m_updateCores;

        //For Reference
        ResMgr* m_resMgr = nullptr;
        GameObjectMgr* m_gameObjectMgr = nullptr;
        RenderMgr* m_renderMgr = nullptr;
        CameraMgr* m_cameraMgr = nullptr;
        LightMgr* m_lightMgr = nullptr;
        SceneMgr* m_sceneMgr = nullptr;
        MemoryMgr* m_memoryMgr = nullptr;
        ScriptMgr* m_scriptMgr = nullptr;

        OGLMgr* m_oglMgr = nullptr;

        bool m_isGenerated = false;
    };
}