#pragma once

#include <vector>
#include <list>
#include "../MacroDef.h"
#include "Base/RenderCoreBase.h"

#define GET_CORE_FUNCTION(CORENAME, core_instance)  \
CORENAME* Get##CORENAME##Core() const {               \
    return static_cast<CORENAME*>(core_instance);   \
}

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
#ifndef CSE_GLOBAL_SCRIPT_DISABLED
    class ScriptMgr;
#endif
    class OGLMgr;
    class ReflectionMgr;
    class InputMgr;
    class CoreBase;
}

namespace CSE {
    class EngineCoreInstance {
    protected:
        EngineCoreInstance();
        virtual ~EngineCoreInstance();

    public:
        GET_CORE_FUNCTION(ResMgr, m_resMgr);
        GET_CORE_FUNCTION(GameObjectMgr, m_gameObjectMgr);
        GET_CORE_FUNCTION(RenderMgr, m_renderMgr);
        GET_CORE_FUNCTION(CameraMgr, m_cameraMgr);
        GET_CORE_FUNCTION(LightMgr, m_lightMgr);
        GET_CORE_FUNCTION(SceneMgr, m_sceneMgr);
        GET_CORE_FUNCTION(MemoryMgr, m_memoryMgr);
#ifndef CSE_GLOBAL_SCRIPT_DISABLED
        GET_CORE_FUNCTION(ScriptMgr, m_scriptMgr);
#endif
        GET_CORE_FUNCTION(ReflectionMgr, m_reflectionMgr);
        GET_CORE_FUNCTION(InputMgr, m_inputMgr);

        void Init(unsigned int width, unsigned int height);
        void Update(float elapsedTime);
        void LateUpdate(float elapsedTime);
        void Render() const;
        void Exterminate();
        void ExterminateWithoutReflectionDefine();
        void ResizeWindow(unsigned int width, unsigned int height);
        void SetDeviceBuffer(unsigned int id);
        virtual void GenerateCores();

        bool IsReady() const {
            return m_isReady;
        }

    protected:
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
#ifndef CSE_GLOBAL_SCRIPT_DISABLED
        ScriptMgr* m_scriptMgr = nullptr;
#endif
        ReflectionMgr* m_reflectionMgr = nullptr;
        InputMgr* m_inputMgr = nullptr;

        OGLMgr* m_oglMgr = nullptr;

        bool m_isGenerated = false;
        bool m_isReady = false;
    };
}