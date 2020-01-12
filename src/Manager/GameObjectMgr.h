#pragma once
#include "../MacroDef.h"
#include "GameObjectContainer.h"

namespace CSE {

    class SGameObject;

    class GameObjectMgr : public GameObjectContainer {
    protected:
    DECLARE_SINGLETONE(GameObjectMgr);

        ~GameObjectMgr();

    public:
        void Init();

        void Update(float elapsedTime);

        void DeleteGameObject(SGameObject* object);

        SGameObject* Find(std::string name) const;

        SGameObject* FindByID(std::string id) const;

        SComponent* FindComponentByID(std::string id) const;

    };
}