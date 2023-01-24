#pragma once

#include "../../Object/SResource.h"
#include <unordered_map>

namespace CSE {

    class GLProgramHandle;

    class SShaderGroup : public SResource {
    public:
        SShaderGroup();

        ~SShaderGroup() override;

        void Exterminate() override;

        const GLProgramHandle* GetHandle(const std::string& pass);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        static std::string&& GetShaderHash(const std::string& vert_path, const std::string& frag_path);

    private:
        std::unordered_map<std::string, GLProgramHandle*> m_handles;
    };
}