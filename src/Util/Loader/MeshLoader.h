//
// Created by ounols on 19. 8. 18.
//

#pragma once


#include "../../Object/SPrefab.h"
#include <string>

class MeshLoader {
public:
    MeshLoader();

    ~MeshLoader();

    static SPrefab* LoadModel(std::string assetName);

private:
    static SPrefab* LoadDAEModel(const char* path);
};



