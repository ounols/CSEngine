#pragma once

#include <unordered_map>
#include "Base/CoreBase.h"
#include "Base/SIContainer.h"
#include "../Object/Base/ReflectionObject.h"

namespace CSE {

    class ReflectionMgr
            : public CoreBase,
              public SIContainer<std::unordered_map<std::string, ReflectionObject*>, ReflectionObject*, std::string> {
    public:
        explicit ReflectionMgr();

        ~ReflectionMgr() override;

        void Init() override;

        void Register(ReflectionObject* object) override;

        void Remove(ReflectionObject* object) override;

        ReflectionObject* Get(std::string index) const override;

        std::unordered_map<std::string, ReflectionObject*> GetAll() const override;

        std::string GetID(ReflectionObject* object) const override;

        int GetSize() const override;

    private:
        std::unordered_map<std::string, ReflectionObject*> m_objects;
    };

}
