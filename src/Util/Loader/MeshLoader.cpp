//
// Created by ounols on 19. 8. 18.
//

#include "MeshLoader.h"
#include "../../Manager/ResMgr.h"
#include "DAE/DAELoader.h"


MeshLoader::MeshLoader() {

}

MeshLoader::~MeshLoader() {

}

SPrefab* MeshLoader::LoadModel(std::string assetName) {

    if(assetName.empty()) return nullptr;

    auto asset = ResMgr::getInstance()->GetAssetReference(assetName);
    std::string path = asset->path;

    switch (asset->type) {
        case AssetMgr::DAE:
            return LoadDAEModel(path.c_str());
            break;

        default:
            return nullptr;
    }


}

SPrefab* MeshLoader::LoadDAEModel(const char* path) {
    DAELoader* loader = new DAELoader(path, nullptr, DAELoader::AUTO);
    SPrefab* prefab = loader->GeneratePrefab();
    SAFE_DELETE(loader);

    return prefab;
}
