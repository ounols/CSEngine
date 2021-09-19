#pragma once

#include <vector>

namespace CSE {

    class STexture;
    class GLProgramHandle;
    class SFrameBuffer;
    class SIRender;

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

        STexture* m_positionTexture = nullptr;
        STexture* m_normalTexture = nullptr;
        STexture* m_albedoTexture = nullptr;
        STexture* m_materialTexture = nullptr;
        unsigned int m_depthRbo = 0;

        GLProgramHandle* m_lightPassHandle = nullptr;

        int m_positonTextureId = -1;
        int m_normalTextureId = -1;
        int m_albedoTextureId = -1;
        int m_materialTextureId = -1;

        int m_width = -1;
        int m_height = -1;
    };
}