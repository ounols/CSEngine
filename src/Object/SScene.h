//
// Created by ounols on 19. 2. 14.
//

#pragma once

#include "../Manager/Base/Scene.h"
#include "SGameObject.h"

namespace CSE {

    class SScene : public Scene {
    public:
        SScene();
        ~SScene() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void Destroy() override;

        SGameObject* GetRoot() {
            return m_root;
        }

    private:
        void InitGameObject(SGameObject* obj);

        void TickGameObject(SGameObject* obj, float elapsedTime);

        void DestroyGameObjects(SGameObject* obj);

    private:
        SGameObject* m_root = nullptr;
    };

}