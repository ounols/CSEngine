//
// Created by ounols on 19. 6. 6.
//
//참조 : https://codingcoding.tistory.com/900
#pragma once


#include <string>
#include <vector>

class AssetMgr {
private:
    enum TYPE { NONE, TEX_2D, TEX_CUBEMAP, MATERIAL, DAE, PREFAB, SCENE };
public:
    struct AssetReference {
        std::string path;
        std::string name;
        std::string extension;
        TYPE type = NONE;
    };
public:
    AssetMgr();
    ~AssetMgr();

    void Exterminate();

    void LoadAssets(bool isPacked);

#ifdef __ANDROID__
    void SetAssetManager(AAssetManager* obj);
    AAssetManager* GetAssetManager();
    void SetEnv(JNIEnv* obj);
    JNIEnv* GetEnv();
#endif

private:
    void ReadDirectory(std::string path);
    void SetType();

private:
    std::vector<AssetReference*> m_assets;

#ifdef __ANDROID__
    AAssetManager* m_assetManager;
    JNIEnv* m_env = nullptr;
#endif
};



