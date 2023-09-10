//
// Created by ounols on 19. 6. 6.
//
//참조 : https://codingcoding.tistory.com/900
#pragma once


#include <string>
#include <list>
#include <unordered_map>
#include "../Util/Loader/ZIP/zip.h"
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <jni.h>
#endif

namespace CSE {

    class SResource;

    class AssetMgr {
    public:
        enum TYPE {
            NONE, TEX_2D, TEX_CUBEMAP, FRAMEBUFFER, MATERIAL, DAE, PREFAB, SCENE, SCRIPT, TXT, SHADER, SHADER_HANDLE, INI
        };

        struct AssetReference {
            std::string name_path;
            std::string path;
            std::string name;
            std::string id;
            std::string hash;
            std::string name_full;
            std::string extension;
            TYPE type = NONE;
            SResource* resource;
        };
    public:
        AssetMgr();

        ~AssetMgr();

        void Exterminate();

        void LoadAssets(bool isPacked);

        AssetReference* GetAsset(const std::string& name) const;

        std::list<AssetMgr::AssetReference*> GetAssets(TYPE type) const;

        static std::string LoadAssetFile(const std::string& path);

        static std::string GetAssetHash(const std::string& path);

#ifdef __CSE_EDITOR__
        std::list<AssetMgr::AssetReference*> GetAllAssets() const {
            return m_assetsList;
        }
#endif

#ifdef __ANDROID__
        void SetAssetManager(AAssetManager* obj);
        AAssetManager* GetAssetManager();
        void SetEnv(JNIEnv* obj);
        JNIEnv* GetEnv();
#endif

    private:
        void ReadDirectory(const std::string& path);
        void ReadPackage(const std::string& path);

        AssetReference* CreateAsset(const std::string& path, const std::string& name_full, std::string name = "");
        AssetReference* CreateAssetFolder(const std::string& path, const std::string& name_full);

        void SetType();

        static AssetReference* AppendSubName(AssetReference* asset, const std::string& sub_name);

    private:
        std::list<AssetReference*> m_assetsList;
        std::unordered_map<std::string, AssetReference*> m_assets;
        zip_t* m_zip = nullptr;
#ifdef __ANDROID__
        AAssetManager* m_assetManager;
        JNIEnv* m_env = nullptr;
#endif
#if defined(__ANDROID__) || defined(IOS)
        std::string m_package_raw;
#endif
    };

}
