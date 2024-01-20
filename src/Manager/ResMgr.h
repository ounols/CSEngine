#pragma once

#include "../MacroDef.h"
#include "Base/SContainerList.h"
#include "../Object/SResource.h"
#include "AssetMgr.h"
#include "Base/CoreBase.h"
#include "Base/SContainerHash.h"
#include <vector>

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace CSE {

    class SObject;

    class GLProgramHandle;

    class SISurface;

    class ResMgr : public CoreBase, public SContainerHash<SResource*> {
    public:
        explicit ResMgr();
        ~ResMgr() override;

    public:

        void Init() override;

        void InitResource();

        void Exterminate();

        void Remove(SResource* m_object) override;

    	template <class TYPE>
		TYPE* GetObjectByHash(const std::string& hash) const;

        SResource* GetSResource(std::string name) const;

		int GetStringHash(const std::string& str);

        AssetMgr::AssetReference* GetAssetReference(std::string name) const;

        std::list<AssetMgr::AssetReference*> GetAssetReferences(AssetMgr::TYPE type) const;

#ifdef __CSE_EDITOR__
        std::list<AssetMgr::AssetReference*> GetAllAssetReferences() const {
            return m_assetManager->GetAllAssets();
        }
#endif

#ifdef __ANDROID__
        void SetAssetManager(AAssetManager* obj);
        AAssetManager* GetAssetManager();
        void SetEnv(JNIEnv* obj);
        JNIEnv* GetEnv();
#endif

    private:
        AssetMgr* m_assetManager = nullptr;
		std::vector<std::string> m_stringIds;

    public:
        friend class AssetMgr;
    };

    template <class TYPE>
    TYPE* ResMgr::GetObjectByHash(const std::string& hash) const {
		if(m_objects.count(hash) <= 0) return nullptr;

		return static_cast<TYPE*>(Get(hash));
    }
}
