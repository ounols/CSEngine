#pragma once

#include <vector>

namespace CSE {

    class STexture;
    class GLProgramHandle;
    class SFrameBuffer;
    class SIRender;
    class ResMgr;

    class SGBuffer {
    public:
        SGBuffer();
        ~SGBuffer();

        void GenerateGBuffer(int width, int height);
        void AttachGeometryFrameBuffer() const;
        void AttachGeometryFrameBuffer(int target) const;
        void ResizeGBuffer(int width, int height);
        void ReleaseGBuffer();
        void BindLightPass(GLProgramHandle* lightPassHandle);
        void AttachLightPass() const;
        void AttachLightPassTexture(int textureLayout = 0) const;
        void RenderLightPass() const;
        const std::vector<SIRender*>& GetRendersLayer() const;
        void PushBackToLayer(SIRender* render);
        void RemoveToLayer(SIRender* render);

        int GetWidth() const;
        int GetHeight() const;
        GLProgramHandle* GetLightPassHandle() const;

    private:
        SFrameBuffer* m_geometryFrameBuffer = nullptr;
        std::vector<SIRender*> m_rendersLayer;

        STexture* m_firstTexture = nullptr;
        STexture* m_secondTexture = nullptr;
        STexture* m_depthTexture = nullptr;

        GLProgramHandle* m_lightPassHandle = nullptr;

        int m_firstTextureId = -1;
        int m_secondTextureId = -1;
        int m_depthTextureId = -1;

        int m_width = -1;
        int m_height = -1;

        ResMgr* m_resMgr = nullptr;
    };
}