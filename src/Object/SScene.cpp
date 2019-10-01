//
// Created by ounols on 19. 2. 14.
//

#include "SScene.h"

SScene::SScene() {
    m_root = new SGameObject("__ROOT_OF_SCENE__");
}

SScene::~SScene() {

}

void SScene::Init() {
    InitGameObject(m_root);
}

void SScene::Tick(float elapsedTime) {
    TickGameObject(m_root, elapsedTime);
}

void SScene::Destroy() {

}

void SScene::InitGameObject(SGameObject* obj) {
    obj->Init();
    for(auto child : obj->GetChildren()) {
        InitGameObject(child);
    }
}

void SScene::TickGameObject(SGameObject* obj, float elapsedTime) {
    obj->Tick(elapsedTime);

    for(auto child : obj->GetChildren()) {
        TickGameObject(child, elapsedTime);
    }
}
