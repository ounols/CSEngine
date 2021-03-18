#include "MainProc.h"
#include "../MacroDef.h"
#include "MemoryMgr.h"
#include "ResMgr.h"
#include "GameObjectMgr.h"
#include "RenderMgr.h"
#include "LightMgr.h"
#include "SceneMgr.h"


#include "../Sample/FirstDemoScene.h"
#include "ScriptMgr.h"
#include "../Object/SScene.h"
#include "../Util/Loader/SCENE/SSceneLoader.h"
#include "../Util/AssetsDef.h"

using namespace CSE;

MainProc::MainProc() {
    m_oglMgr = new OGLMgr();
    m_scriptMgr = new ScriptMgr();
}


MainProc::~MainProc() {
    Exterminate();
}


void MainProc::Init(GLuint width, GLuint height) {

    m_oglMgr->setupEGLGraphics(width, height);
    ResizeWindow(width, height);

    m_scriptMgr->Init();
    GameObjectMgr::getInstance()->Init();

//    SScene* scene = SSceneLoader::LoadScene(CSE::AssetsPath() + "Scene/test_scene.scene");
//    SceneMgr::getInstance()->SetScene(scene);
    SceneMgr::getInstance()->SetScene(new FirstDemoScene());



}


void MainProc::Update(float elapsedTime) {
    GameObjectMgr::getInstance()->Update(elapsedTime);
    SceneMgr::getInstance()->Tick(elapsedTime);

}


void MainProc::Render(float elapsedTime) const {

    m_oglMgr->Render(elapsedTime);
    RenderMgr::getInstance()->Render(elapsedTime);
}


void MainProc::Exterminate() {

    SAFE_DELETE(m_oglMgr);
    MemoryMgr::getInstance()->ExterminateObjects(true);

    ResMgr::delInstance();
    GameObjectMgr::delInstance();
    RenderMgr::delInstance();
    CameraMgr::delInstance();
    LightMgr::delInstance();
    SceneMgr::delInstance();

    MemoryMgr::delInstance();

    SAFE_DELETE(m_scriptMgr);

}

void MainProc::ResizeWindow(GLuint width, GLuint height) const {

    m_oglMgr->ResizeWindow(width, height);

}
