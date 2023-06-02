#pragma once

#include "../../Util/Render/SRenderGroup.h"

namespace CSE {

    class STexture;
    class GLProgramHandle;
    class SFrameBuffer;

    class SdfRenderGroup : public SRenderGroup {
    public:
        explicit SdfRenderGroup(const RenderMgr& renderMgr);

        ~SdfRenderGroup() override;

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        void RenderAll(const CameraBase& camera) const override;

        void Exterminate() override;

        void BindShaderUniforms(const GLProgramHandle& handle) const;

    private:
        int m_envSize = 16;
        vec3 m_nodeSize = vec3{ 7.f, 3.f, 7.f };
        vec2 m_mapSize = vec2{ -1, -1 };
        float m_nodeSpace = 0.5f;
        unsigned int m_envSizeId = 0;
        unsigned int m_nodeSizeId = 0;
        unsigned int m_nodeSpaceId = 0;

        SFrameBuffer* m_sdfMapBuffer = nullptr;
        STexture* m_mapTexture = nullptr;
        unsigned int m_mapTextureId = 0;

        //For Implement Only (must remove!)
        STexture* m_testTexture = nullptr;
        unsigned int m_testTextureId = 0;
        GLProgramHandle* m_sdfHandle = nullptr;
    };

}
