//
// Created by ounols on 19. 6. 6.
//

#include "AssetMgr.h"
#include "../MacroDef.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include <iostream>

#ifdef __linux__
#include <sys/types.h>
#include <dirent.h>


#elif WIN32
#include <windows.h>
#endif
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <Manager/ResMgr.h>
#endif

AssetMgr::AssetMgr() {

}

AssetMgr::~AssetMgr() {
    Exterminate();
}


void AssetMgr::Exterminate() {
    for(auto asset : m_assets) {
        SAFE_DELETE(asset);
    }

#ifdef __ANDROID__
    SAFE_DELETE(m_assetManager);
#endif
}

void AssetMgr::LoadAssets(bool isPacked) {

    if(!isPacked) {
        ReadDirectory(CSE::AssetsPath());


    }
}

void AssetMgr::ReadDirectory(std::string path) {
#ifdef __linux__ //======================================

    DIR* dirp = opendir(path.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != nullptr) {

        std::string name = dp->d_name;

        if(dp->d_type == DT_DIR) {
            if(name == "." || name == "..") continue;

            ReadDirectory(path + name + '/');
            continue;
        }

        AssetReference* asset = new AssetReference();
        asset->name = name;
        asset->path = path + name;

        auto name_strs = split(name, '.');
        asset->extension = name_strs[name_strs.size() - 1];

        m_assets.push_back(asset);
        std::cout << "[pkg] " << asset->name << " (" << asset->extension << ")\n";
    }
    closedir(dirp);

    return;

#endif //================================================
#ifdef WIN32 //==========================================
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
#endif //================================================
}

void AssetMgr::SetType() {

    for(auto asset : m_assets) {
        std::string type_str = asset->extension;

        //texture data
        if(type_str == "jpg" || type_str == "png" || type_str == "dds" || type_str == "hdr") {
            asset->type = TEX_2D;
            continue;
        }

        //cube map texture data
        if(type_str == "cbmap") {
            asset->type = TEX_CUBEMAP;
            continue;
        }
    }

}


#ifdef __ANDROID__

void AssetMgr::SetAssetManager(AAssetManager* obj) {
    m_assetManager = obj;
}


AAssetManager * AssetMgr::GetAssetManager() {
    return m_assetManager;
}

void AssetMgr::SetEnv(JNIEnv *obj) {
    m_env = obj;
}

JNIEnv *AssetMgr::GetEnv() {
    return m_env;
}

#endif