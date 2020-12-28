#pragma once

#include "../MacroDef.h"
#include "Base/SContainer.h"
#include "../Object/SResource.h"
#include "AssetMgr.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

#define RESMGR ResMgr::getInstance()

namespace CSE {

    class SObject;

    class GLProgramHandle;

    class SISurface;

    class ResMgr {
    private:
        ~ResMgr();

    public:
    DECLARE_SINGLETONE(ResMgr);

        void Init();

        void Exterminate();

        void Register(SResource* m_object);

        void Remove(SResource* m_object);

        template <class TYPE>
        TYPE* GetObject(std::string name) const;

        SResource* GetSResource(std::string name) const;

        int GetID(SResource* object) const;

        int GetSize() const;

        bool IsEmpty() const;

		int GetStringId(std::string str);

        std::string RemoveDuplicatingName(std::string name) const;

        AssetMgr::AssetReference* GetAssetReference(std::string name) const;

        std::vector<AssetMgr::AssetReference*> GetAssetReferneces(AssetMgr::TYPE type) const;

#ifdef __ANDROID__
        void SetAssetManager(AAssetManager* obj);
        AAssetManager* GetAssetManager();
        void SetEnv(JNIEnv* obj);
        JNIEnv* GetEnv();
#endif


    private:
        std::vector<SResource*> m_resources;
        AssetMgr* m_assetManager;
		std::vector<std::string> m_stringIds;
    };


    template <class TYPE>
    TYPE* ResMgr::GetObject(std::string name) const {
        for (auto resource : m_resources) {
            if (dynamic_cast<TYPE*>(resource)) {
                if (name == resource->GetName())
                    return static_cast<TYPE*>(resource);
            }
        }

        return nullptr;
    }
}