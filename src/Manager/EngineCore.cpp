//
// Created by ounol on 2021-03-22.
//

#include "EngineCore.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "OGLMgr.h"
#include "ScriptMgr.h"
#include "CameraMgr.h"
#include "LightMgr.h"
#include "Render/RenderMgr.h"
#include "SceneMgr.h"
#include "MemoryMgr.h"
#include "../Sample/FirstDemoScene.h"
#include "../Sample/WebDemoScene.h"
#include "../Util/Loader/SCENE/SSceneLoader.h"
#include "../Util/AssetsDef.h"

#ifdef __EMSCRIPTEN__
#include "../Sample/WebDemoScene.h"
#endif

using namespace CSE;
IMPLEMENT_SINGLETON(EngineCore);

EngineCore::EngineCore() = default;

EngineCore::~EngineCore() = default;


