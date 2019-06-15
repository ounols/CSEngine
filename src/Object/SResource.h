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
    static T* Create(std::string name) {
        T* object = new T();
        SResource* res = object;

        res->SetResource(name);
        return object;
    }

    template <class T>
    static T* Create(const AssetMgr::AssetReference* asset) {
        if(asset == nullptr) return nullptr;
        T* object = new T();
        SResource* res = object;

        res->SetResource(asset->name);
        return object;
    }

protected:
    virtual void Init(const AssetMgr::AssetReference* asset) = 0;

private:
    std::string m_name;
    bool m_isInited = false;

};



