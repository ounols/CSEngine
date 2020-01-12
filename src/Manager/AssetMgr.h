//
// Created by ounols on 19. 6. 6.
//
//참조 : https://codingcoding.tistory.com/900
#pragma once


#include <string>
#include <vector>

namespace CSE {

    class AssetMgr {
    public:
        enum TYPE {
            NONE, TEX_2D, TEX_CUBEMAP, MATERIAL, DAE, PREFAB, SCENE, SCRIPT, TXT, SHADER, SHADER_HANDLE, INI
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

#ifdef __ANDROID__
        void SetAssetManager(AAssetManager* obj);
        AAssetManager* GetAssetManager();
        void SetEnv(JNIEnv* obj);
        JNIEnv* GetEnv();
#endif

    private:
        void ReadDirectory(std::string path);

        AssetReference* CreateAsset(std::string path, std::string name_full, std::string name = "");

        void SetType();

        AssetReference* AppendSubName(AssetReference* asset, std::string sub_name);

    private:
        std::vector<AssetReference*> m_assets;

#ifdef __ANDROID__
        AAssetManager* m_assetManager;
        JNIEnv* m_env = nullptr;
#endif
    };

}