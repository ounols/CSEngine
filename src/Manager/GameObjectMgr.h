#pragma once
#include "../MacroDef.h"
#include "GameObjectContainer.h"
#include "Base/CoreBase.h"

namespace CSE {

    class SGameObject;

    class GameObjectMgr : public GameObjectContainer, public CoreBase {
    public:
        explicit GameObjectMgr();
        ~GameObjectMgr();

    public:
        void Init() override;

        void Update(float elapsedTime) override;

        void DeleteGameObject(SGameObject* object);

        SGameObject* Find(std::string name) const;

        SGameObject* FindByID(std::string id) const;

        SComponent* FindComponentByID(std::string id) const;

    };
}