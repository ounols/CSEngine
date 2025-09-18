//
// Created by ounols on 19. 6. 6.
//

#include "AssetMgr.h"
#include "../MacroDef.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include "../Util/SafeLog.h"
#include "../Util/Loader/XML/XML.h"
#include "EngineCore.h"
#include "ResMgr.h"
#include <iostream>
#include <stack>

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
    m_assets.clear();

    for (auto& asset: m_assetsList) {
        SAFE_DELETE(asset);
    }

#ifdef __ANDROID__
    SAFE_DELETE(m_assetManager);
#endif

    zip_close(m_zip);
    m_assetsList.clear();
}

void AssetMgr::LoadAssets(bool isPacked) {

    if (!isPacked) {
        ReadDirectory(CSE::NativeAssetsPath());
    } else {
        ReadPackage(CSE::NativeAssetsPath() + "Assets.zip");
    }
    SetType();
}

AssetMgr::AssetReference* AssetMgr::GetAsset(const std::string& name) const {
    // Get an asset by hash
    if (m_assets.count(name) > 0) return m_assets.at(name);

    std::string lowerName = name;
    make_lower(lowerName);
    for (const auto& asset: m_assetsList) {
        if (make_lower_copy(asset->name) == lowerName) return asset;
        if (make_lower_copy(asset->id) == lowerName) return asset;
        if (make_lower_copy(asset->name_path) == lowerName) return asset;
        if (make_lower_copy(asset->name_full) == lowerName) return asset;
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
        AssetReference* assetReference = CreateAsset(path, name);

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
#ifdef __CSE_EDITOR__
            CreateAssetFolder(path, name);
#endif
            ReadDirectory(path + name + '/');
            continue;
        }

        AssetReference* asset = CreateAsset(path, name);
        if (asset == nullptr) continue;
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
#ifdef __CSE_EDITOR__
                CreateAssetFolder(path, name);
#endif
                ReadDirectory(path + name + '/');
                continue;
            }

            AssetReference* asset = CreateAsset(path, name);
            if (asset == nullptr) continue;
            std::cout << "[pkg] " << asset->name << " (" << asset->extension << ")\n";

        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
#endif //================================================
}

void AssetMgr::ReadPackage(const std::string& path) {
#if defined(__ANDROID__) || defined(IOS)
    if(m_zip == nullptr) {
        SafeLog::Log(path.c_str());
        m_package_raw = OpenNativeAssetsTxtFile(path);
        m_zip = zip_stream_open(m_package_raw.c_str(), m_package_raw.length(), 0, 'r');
    }
#else
    if (m_zip == nullptr) m_zip = zip_open(path.c_str(), 0, 'r');
#endif
    int zipSize = zip_entries_total(m_zip);
    for (int i = 0; i < zipSize; ++i) {
        zip_entry_openbyindex(m_zip, i);
        {
            const char* name = zip_entry_name(m_zip);
            int isdir = zip_entry_isdir(m_zip);

            std::string name_str = name;
            if(name_str.find("__MACOSX/") != std::string::npos) continue;
            if(isdir != 0) name_str = name_str.substr(0, name_str.size() - 1);
            auto rFindIndex = name_str.rfind('/');
            std::string name_cropped = name_str;
            std::string path_str = "";
            if (rFindIndex != std::string::npos) {
                name_cropped = name_str.substr(rFindIndex + 1);
                path_str = name_str.substr(0, rFindIndex + 1);
            }
            if (isdir != 0) {
#ifdef __CSE_EDITOR__
                CreateAssetFolder(path_str, name_cropped);
#endif
                continue;
            }
            AssetReference* asset = CreateAsset(path_str, name_cropped);
            if (asset == nullptr) continue;
            std::cout << "(Packed)[pkg] " << asset->name << " (" << asset->extension << ")\n";
        }
        zip_entry_close(m_zip);
    }
}

AssetMgr::AssetReference*
AssetMgr::CreateAsset(const std::string& path, const std::string& name_full, std::string name) {
    auto asset = new AssetReference();
    asset->path = path;
    asset->name_path = path + name_full;
    asset->id = "File:" + asset->name_path.substr(CSE::AssetsPath().size());
    asset->name_full = name_full;

    if (name.empty()) {
        name = name_full;
        auto name_strs = split(name, '.');
        asset->extension = name_strs[name_strs.size() - 1];
        asset->name = name.substr(0, name.rfind('.'));
    } else {
        asset->name = name;
    }

    // Ignore meta files
    if (asset->extension == "meta") {
        SAFE_DELETE(asset);
        return nullptr;
    }

    asset->hash = GetAssetHash(asset->name_path);

    m_assets.insert(std::pair<std::string, AssetReference*>(asset->hash, asset));
    m_assetsList.push_back(asset);

    return asset;
}

AssetMgr::AssetReference* AssetMgr::CreateAssetFolder(const std::string& path, const std::string& name_full) {
    auto asset = new AssetReference();
    asset->path = path;
    asset->name_path = path + name_full;
    asset->extension = "/\\?folder";
    asset->id = "Folder:" + asset->name_path.substr(CSE::AssetsPath().size());
    asset->name_full = name_full;
    asset->name = name_full;

    asset->hash = "FOLDER" + GetRandomHash(10);

    m_assets.insert(std::pair<std::string, AssetReference*>(asset->hash, asset));
    m_assetsList.push_back(asset);

    return asset;
}

void AssetMgr::SetType() {

    for (const auto asset: m_assetsList) {
        std::string type_str = asset->extension;
        make_lower(type_str);

        //차후 확장자 리스트를 제작하여 최적화
        //texture data
        if (type_str == "jpg" || type_str == "png" || type_str == "dds" || type_str == "hdr") {
            asset->type = TEX_2D;
            asset->name += ".texture";
            asset->class_type = "STexture";
            continue;
        }

        //cube map texture data
        if (type_str == "cbmap") {
            asset->type = TEX_CUBEMAP;
            asset->name += ".textureCubeMap";
            asset->class_type = "STexture";
            continue;
        }

        // framebuffer data
        if (type_str == "framebuffer") {
            asset->type = FRAMEBUFFER;
            asset->name += ".frameBuffer";
            asset->class_type = "SFrameBuffer";
            continue;
        }

        //material data
        if (type_str == "mat") {
            asset->type = MATERIAL;
            asset->name += ".material";
            asset->class_type = "SMaterial";
            continue;
        }

        //DAE data
        if (type_str == "dae") {
            asset->type = DAE;
            asset->name += ".prefab";
            asset->class_type = "SPrefab";
            {
                const auto& ani = AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name),
                                                "animation");
                const auto& ske = AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name),
                                                "skeleton");
                const auto& mes = AppendSubName(CreateAsset(asset->path, asset->name_full, asset->name), "mesh");

                ani->type = DAE;
                ske->type = DAE;
                mes->type = DAE;
                ani->class_type = "Animation";
                ske->class_type = "Skeleton";
                mes->class_type = "MeshSurface";
            }
            continue;
        }

        //script data
        if (type_str == "nut") {
            asset->type = SCRIPT;
            asset->name += ".script";
            asset->class_type = "SScriptObject";
            continue;
        }

        //shader part data
        if (type_str == "vert" || type_str == "vs") {
            asset->type = SHADER;
            asset->name += ".vert";
            asset->class_type = "GLProgramHandle";
            continue;
        }
        if (type_str == "frag" || type_str == "fs") {
            asset->type = SHADER;
            asset->name += ".frag";
            asset->class_type = "GLProgramHandle";
            continue;
        }

        //shader part data
        if (type_str == "shader") {
            asset->type = SHADER_HANDLE;
            asset->name += ".shader";
            asset->class_type = "SShaderGroup";
            continue;
        }

        //prefab data
        if (type_str == "prefab") {
            asset->type = PREFAB;
            asset->name += ".prefab";
            asset->class_type = "SPrefab";
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

        if (asset->type == NONE) {
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

std::list<AssetMgr::AssetReference*> AssetMgr::GetAssets(TYPE type) const {
    std::list<AssetReference*> result;

    for (const auto& asset: m_assetsList) {
        if (asset->type == type) {
            result.push_back(asset);
        }
    }

    return result;
}

std::string AssetMgr::LoadAssetFile(const std::string& path) {
    if (Settings::IsAssetsPacked() == false)
        return OpenNativeAssetsTxtFile(path);

    std::string result;
    std::string path_convert;
    const auto& zip = CORE->GetResMgrCore()->m_assetManager->m_zip;
    if (path.find("/../") != std::string::npos) {
        auto path_split = split(path, '/');
        std::stack<std::string> path_stack;
        for (const auto& part: path_split) {
            if (part == "..") {
                path_stack.pop();
                continue;
            }
            path_stack.push(part);
        }
        while (!path_stack.empty()) {
            path_convert = path_stack.top() + '/' + path_convert;
            path_stack.pop();
        }
        path_convert = path_convert.substr(0, path_convert.size() - 1);
    } else {
        path_convert = path;
    }
    zip_entry_open(zip, path_convert.c_str());
    {
        char* buf = nullptr;
        size_t bufsize = 0;

        zip_entry_read(zip, (void**) &buf, &bufsize);
        result = std::string(buf, bufsize);
        free(buf);
    }
    zip_entry_close(zip);
    return result;
}

std::string AssetMgr::GetAssetHash(const std::string& path) {
    std::string meta = LoadAssetFile(path + ".meta");
    std::string&& hash = "";
    if (!meta.empty()) {
        const XNode* root = XFILE().loadBuffer(std::move(meta));
        const auto& hashData = root->getNode("hash-data");
        hash = hashData.getAttribute("hash").value;
        SAFE_DELETE(root);
    } else {
        hash = GetRandomHash(16);
        std::string meta = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<CSEMETA version=\"1.0.0\">\n"
                           "<hash-data hash=\"" + hash + "\">\n"
                                                         "\n</hash-data>\n</CSEMETA>";
        if (!Settings::IsAssetsPacked())
            SaveTxtFile(path + ".meta", meta);
    }
    return hash;
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
