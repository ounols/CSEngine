#pragma once

#include "../../Object/SResource.h"

namespace CSE {

    class GLProgramHandle;

    class SShaderGroup : public SResource {
    public:
        SShaderGroup();

        ~SShaderGroup() override;

        void Exterminate() override;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        std::unordered_map<std::string, GLProgramHandle*> m_handles;
    };
}