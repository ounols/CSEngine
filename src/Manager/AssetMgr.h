//
// Created by ounols on 19. 6. 6.
//
//참조 : https://codingcoding.tistory.com/900
#pragma once


#include <string>
#include <vector>
#include "../Util/Loader/ZIP/zip.h"
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <jni.h>
#endif

namespace CSE {

    class AssetMgr {
    public:
        enum TYPE {
            NONE, TEX_2D, TEX_CUBEMAP, FRAMEBUFFER, MATERIAL, DAE, PREFAB, SCENE, SCRIPT, TXT, SHADER, SHADER_HANDLE, INI
        };

        struct AssetReference {
            std::string path;
            std::string name;
            std::string id;
            std::string name_full;
            std::string extension;
            TYPE type = NONE;
        };
    public:
        AssetMgr();

        ~AssetMgr();

        void Exterminate();

        void LoadAssets(bool isPacked);

        AssetReference* GetAsset(std::string name) const;

        std::vector<AssetReference*> GetAssets(TYPE type) const;

        static std::string LoadAssetFile(const std::string& path);

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

        void SetType();

        static AssetReference* AppendSubName(AssetReference* asset, const std::string& sub_name);

    private:
        std::vector<AssetReference*> m_assets;
        zip_t* m_zip = nullptr;
#ifdef __ANDROID__
        AAssetManager* m_assetManager;
        JNIEnv* m_env = nullptr;
        std::string m_package_raw;
#endif
    };

}