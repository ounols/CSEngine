//
// Created by ounols on 19. 2. 14.
//

#pragma once

#include "../Manager/Base/Scene.h"
#include "SGameObject.h"

class SScene : public Scene {
public:
    SScene();
    ~SScene();

    void Init() override;

    void Tick(float elapsedTime) override;

    void Destroy() override;

    SGameObject* GetRoot() {
        return m_root;
    }

private:
    SGameObject* m_root = nullptr;
};


