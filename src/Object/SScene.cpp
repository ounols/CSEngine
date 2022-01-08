//
// Created by ounols on 19. 2. 14.
//

#include "SScene.h"

using namespace CSE;

SScene::SScene() {
    m_root = new SGameObject("__ROOT_OF_SCENE__");
}

SScene::~SScene() = default;

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
    const auto& children = obj->GetChildren();
    for (const auto& child : children) {
        InitGameObject(child);
    }
}

void SScene::TickGameObject(SGameObject* obj, float elapsedTime) {
    obj->Tick(elapsedTime);
    const auto& children = obj->GetChildren();
    for (const auto& child : children) {
        TickGameObject(child, elapsedTime);
    }
}
