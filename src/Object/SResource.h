//
// Created by ounols on 19. 6. 10.
//

#pragma once

#include <string>
#include "../Manager/AssetMgr.h"
#include "../SObject.h"

class SResource : public SObject {
public:
    SResource();
    virtual ~SResource();

    void SetName(std::string name);

    const char* GetName() const {
        return m_name.c_str();
    }

    void SetResource(std::string name);
    void SetResource(const AssetMgr::AssetReference* asset);

    template <class T>
    static T* Create(std::string name, bool isForceCopy = false) {
        if(!isForceCopy) {
            SResource* res = GetResource(name);
            if(res != nullptr) return static_cast<T*>(res);
        }
        T* object = new T();
        SResource* res = object;

        res->SetResource(name);
        return object;
    }

    template <class T>
    static T* Create(const AssetMgr::AssetReference* asset, bool isForceCopy = false) {
        if(asset == nullptr) return nullptr;
        if(!isForceCopy) {
            SResource* res = GetResource(asset->name);
            if(res != nullptr) return static_cast<T*>(res);
        }
        T* object = new T();
        SResource* res = object;

        res->SetResource(asset->name);
        return object;
    }

protected:
    virtual void Init(const AssetMgr::AssetReference* asset) = 0;
private:
    static SResource* GetResource(std::string name);
private:
    std::string m_name;
    bool m_isInited = false;

};



