//
// Created by ounols on 19. 4. 28.
//

#pragma once

#include <string>
#include "../../../Object/SScene.h"


class SSceneLoader {
public:
    SSceneLoader();

    ~SSceneLoader();

    static bool SaveScene(SScene* scene, std::string path);
    static bool SavePrefab(SGameObject* root, std::string path);

private:
    static std::string GetGameObjectValue(SGameObject* obj);

    static bool Save(std::string buf, std::string path);
};