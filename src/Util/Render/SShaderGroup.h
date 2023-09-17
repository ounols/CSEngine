#pragma once

#include "../../Object/SResource.h"
#include <unordered_map>

namespace CSE {

    class GLProgramHandle;

    class SShaderGroup : public SResource {
    public:
        RESOURCE_DEFINE_CONSTRUCTOR(SShaderGroup);

        ~SShaderGroup() override;

        void Exterminate() override;

        const GLProgramHandle* GetHandle(const std::string& pass) const;
        const GLProgramHandle* GetHandleByMode(int mode) const;
        const GLProgramHandle* GetGeometryHandle() const {
            return m_geometryHandle;
        }
        const GLProgramHandle* GetForwardHandle() const {
            return m_forwardHandle;
        }
        const GLProgramHandle* GetDepthOnlyHandle() const {
            return m_depthOnlyHandle;
        }

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        static std::string GetShaderHash(const std::string& vert_path, const std::string& frag_path);

    private:
        std::unordered_map<std::string, GLProgramHandle*> m_handles;
        GLProgramHandle* m_geometryHandle = nullptr;
        GLProgramHandle* m_forwardHandle = nullptr;
        GLProgramHandle* m_depthOnlyHandle = nullptr;
    };
}