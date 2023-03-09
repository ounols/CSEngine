#pragma once
#include <vector>
#include "../Object/SGameObject.h"
#include "Base/SContainerHash.h"

namespace CSE {

    class GameObjectContainer : public SContainerHash<SGameObject*> {
    protected:
        GameObjectContainer();

        ~GameObjectContainer() override;

    };

}