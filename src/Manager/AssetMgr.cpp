//
// Created by ounols on 19. 6. 6.
//

#include "AssetMgr.h"
#include "../MacroDef.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include "../Util/SafeLog.h"
#include "EngineCore.h"
#include "ResMgr.h"
#include <iostream>

#if defined(__linux__) || defined(__APPLE_CC__)
#include <sys/types.h>
#include <dirent.h>

#elif _WIN32
#include <windows.h>
#endif
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <Manager/ResMgr.h>
#include <Util/SafeLog.h>

#endif
using namespace CSE;

AssetMgr::AssetMgr() = default;

AssetMgr::~AssetMgr() {
    Exterminate();
}


void AssetMgr::Exterminate() {
    for (auto asset : m_assets) {
        SAFE_DELETE(asset);
    }

#ifdef __ANDROID__
    SAFE_DELETE(m_assetManager);
#endif

    zip_close(m_zip);
}

void AssetMgr::LoadAssets(bool isPacked) {

    if (!isPacked) {
        ReadDirectory(CSE::NativeAssetsPath());
    }
    else {
        ReadPackage(CSE::NativeAssetsPath() + "Assets.zip");
    }
    SetType();
}

AssetMgr::AssetReference* AssetMgr::GetAsset(std::string name) const {
    make_lower(name);
    for (auto asset : m_assets) {
        if (make_lower_copy(asset->name) == name) return asset;
        if (make_lower_copy(asset->id) == name) return asset;
        if (make_lower_copy(asset->path) == name) return asset;
        if (make_lower_copy(asset->name_full) == name) return asset;
    }
    auto errorLog = "[Assets Warning] " + name + " does not exist.";
    SafeLog::Log(errorLog.c_str());
    return nullptr;
}

void AssetMgr::ReadDirectory(const std::string& path) {
#ifdef __ANDROID__
    return;
    AAssetDir* assetDir = AAssetManager_openDir(m_assetManager, "");
    const char* filename = (const char*)NULL;
    SafeLog::Log("ReadDirectory");
    while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
        AAsset* asset = AAssetManager_open(m_assetManager, filename, AASSET_MODE_STREAMING);

        std::string name = filename;
        SafeLog::Log(name.c_str());
        AssetReference* assetReference = CreateAsset(path + name, name);

        AAsset_close(asset);
    }
    AAssetDir_close(assetDir);
#elif defined(__linux__) || defined(__APPLE_CC__) //======================================

    DIR* dirp = opendir(path.c_str());
    struct dirent* dp;
    while ((dp = readdir(dirp)) != nullptr) {

        std::string name = dp->d_name;

        if (dp->d_type == DT_DIR) {
            if (name == "." || name == "..") continue;

            ReadDirectory(path + name + '/');
            continue;
        }

        AssetReference* asset = CreateAsset(path + name, name);
        std::cout << "[pkg] " << asset->name << " (" << asset->extension << ")\n";
    }
    closedir(dirp);

    return;

#endif //================================================
#ifdef _WIN32 //==========================================
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(std::string(path + '*').c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
			std::string name = data.cFileName;

			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (name == "." || name == "..") continue;

				ReadDirectory(path + name + '/');
				continue;
			}

			AssetReference* asset = CreateAsset(path + name, name);
			std::cout << "[pkg] " << asset->name << " (" << asset->extension << ")\n";

        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
#endif //================================================
}

void AssetMgr::ReadPackage(const std::string& path) {
#ifdef __ANDROID__
    if(m_zip == nullptr) {
        SafeLog::Log(path.c_str());
        m_package_raw = OpenNativeAssetsTxtFile(path);
        m_zip = zip_stream_open(m_package_raw.c_str(), m_package_raw.length(), 0, 'r');
    }
#else
    if(m_zip == nullptr) m_zip = zip_open(path.c_str(), 0, 'r');
#endif
    int zipSize = zip_entries_total(m_zip);
    for (int i = 0; i < zipSize; ++i) {
        zip_entry_openbyindex(m_zip, i);
        {
            const char *name = zip_entry_name(m_zip);
            std::string name_str = name;
            auto rFindIndex = name_str.rfind('/');
            std::string name_cropped = (rFindIndex == std::string::npos) ? name_str : name_str.substr(rFindIndex + 1);
            int isdir = zip_entry_isdir(m_zip);
            if(isdir != 0) continue;
            AssetReference* asset = CreateAsset(name, name_cropped);
            std::cout << "(Packed)[pkg] " << asset->name << " (" << asset->extension << ")\n";
        }
        zip_entry_close(m_zip);
    }

}

AssetMgr::AssetReference* AssetMgr::CreateAsset(const std::string& path, const std::string& name_full, std::string name) {
    auto asset = new AssetReference();
    asset->path = path;
    asset->id = "File:" + path.substr(CSE::AssetsPath().size());
    asset->name_full = name_full;

    if (name.empty()) {
        name = name_full;
        auto name_strs = split(name, '.');
        asset->extension = name_strs[name_strs.size() - 1];
        asset->name = name.substr(0, name.rfind('.'));
    } else {
        asset->name = name;
    }


    m_assets.push_back(asset);

    return asset;
}

void AssetMgr::SetType() {

	for (int i = 0; i < m_assets.size(); ++i) {
		const auto asset = m_assets[i];
        std::string type_str = asset->extension;
        make_lower(type_str);

        //차후 확장자 리스트를 제작하여 최적화
        //texture data
        if (type_str == "jpg" || type_str == "png" || type_str == "dds" || type_str == "hdr") {
            asset->type = TEX_2D;
            asset->name += ".texture";
            continue;
        }

        //cube map texture data
        if (type_str == "cbmap") {
            asset->type = TEX_CUBEMAP;
            asset->name += ".textureCubeMap";
            continue;
        }

        // framebuffer data
        if (type_str == "framebuffer") {
            asset->type = FRAMEBUFFER;
            asset->name += ".frameBuffer";
            continue;
        }

        //material data
        if (type_str == "mat") {
            asset->type = MATERIAL;
            asset->name += ".material";
            continue;
        }

        //DAE data
        if (type_str == "dae") {
            asset->type = DAE;
            asset->name += ".prefab";
            {
                AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name), "animation")->type = DAE;
                AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name), "skeleton")->type = DAE;
                AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name), "mesh")->type = DAE;
            }
            continue;
        }

        //script data
        if (type_str == "nut") {
            asset->type = SCRIPT;
            asset->name += ".script";
            continue;
        }

        //shader part data
        if (type_str == "vert" || type_str == "vs") {
            asset->type = SHADER;
            asset->name += ".vert";
            continue;
        }
        if (type_str == "frag" || type_str == "fs") {
            asset->type = SHADER;
            asset->name += ".frag";
            continue;
        }

        //shader part data
        if (type_str == "shader") {
            asset->type = SHADER_HANDLE;
            asset->name += ".shader";
            continue;
        }

        //prefab data
        if (type_str == "prefab") {
            asset->type = PREFAB;
            asset->name += ".prefab";
            continue;
        }

        //scene data
        if (type_str == "scene") {
            asset->type = SCENE;
            asset->name += ".scene";
            continue;
        }

        //ini data
        if (type_str == "ini") {
            asset->type = INI;
            asset->name += ".ini";
            continue;
        }

		if(asset->type == NONE) {
			//txt data
			asset->type = TXT;
			asset->name += ".text";
		}

    }

}

AssetMgr::AssetReference* AssetMgr::AppendSubName(AssetMgr::AssetReference* asset, const std::string& sub_name) {
    std::string sub = '?' + sub_name;
    asset->id += sub;
    asset->name += sub;
    return asset;
}

std::vector<AssetMgr::AssetReference*> AssetMgr::GetAssets(AssetMgr::TYPE type) const {
    std::vector<AssetReference*> result;

    for(const auto& asset : m_assets) {
        if(asset->type == type) {
            result.push_back(asset);
        }
    }

    return result;
}

std::string AssetMgr::LoadAssetFile(const std::string& path) {
    if(Settings::IsAssetsPacked() == false)
        return OpenNativeAssetsTxtFile(path);

    std::string result;
    const auto& zip = CORE->GetResMgrCore()->m_assetManager->m_zip;
    zip_entry_open(zip, path.c_str());
    {
        char* buf = nullptr;
        size_t bufsize = 0;

        zip_entry_read(zip, (void **)&buf, &bufsize);
        result = std::string(buf, bufsize);
        free(buf);
    }
    zip_entry_close(zip);
    return result;
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