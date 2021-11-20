#pragma once
#include <vector>
#include "../Object/SGameObject.h"
#include "Base/SContainer.h"

namespace CSE {

    class GameObjectContainer : public SContainer<SGameObject*> {
    protected:
        GameObjectContainer();

        ~GameObjectContainer() override;

    };

}