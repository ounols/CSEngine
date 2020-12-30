#pragma once
#include "Base/Scene.h"
#include "../MacroDef.h"

namespace CSE {

    class SceneMgr {
    public:
    DECLARE_SINGLETONE(SceneMgr);

        ~SceneMgr();

        void Init() const;

        void Tick(float elapsedTime) const;

        void SetScene(Scene* scene);

        Scene* GetCurrentScene() const;

    private:
        Scene* m_scene = nullptr;
    };

}