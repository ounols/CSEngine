#pragma once

#include <queue>
#include "../MacroDef.h"
#include "GameObjectContainer.h"
#include "Base/CoreBase.h"

namespace CSE {

    class SGameObject;

    class GameObjectMgr : public GameObjectContainer, public CoreBase {
    public:
        explicit GameObjectMgr();
        ~GameObjectMgr() override;

    public:
        void Init() override;

        void Update(float elapsedTime) override;

        void DestroyQueuedObject();

        void AddDestroyObject(SGameObject* object);

        SGameObject* Find(const std::string& name) const;

        SGameObject* FindByID(const std::string& id) const;

        SComponent* FindComponentByID(const std::string& id) const;

    private:
        std::queue<SGameObject*> m_destroyObjectsQueue;

    };
}