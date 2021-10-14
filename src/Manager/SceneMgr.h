#pragma once
#include "Base/Scene.h"
#include "../MacroDef.h"
#include "Base/CoreBase.h"

namespace CSE {

    class SceneMgr : public CoreBase {
    public:
        explicit SceneMgr();
        ~SceneMgr() override;

        void Init() override;

        void Update(float elapsedTime) override;

        void SetScene(Scene* scene);

        Scene* GetCurrentScene() const;

    private:
        Scene* m_scene = nullptr;
    };

}