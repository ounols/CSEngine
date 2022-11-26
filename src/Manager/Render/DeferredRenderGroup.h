#pragma once

#include <unordered_map>
#include "../../Util/Render/SRenderGroup.h"

namespace CSE {

    class SGBuffer;
    class LightMgr;

    class DeferredRenderGroup  : public SRenderGroup {
    private:
        typedef std::unordered_map<GLProgramHandle*, SGBuffer*> GBufferLayer;

    public:
        explicit DeferredRenderGroup(const RenderMgr& renderMgr);

        ~DeferredRenderGroup() override;

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        /**
         * Deferred rendering for each G-Buffer object.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderAll(const CameraBase& camera) const override;

        /**
         * Deferred rendering for each G-Buffer object.
         * @param camera Structure containing information values of the camera to be drawn.
         * @param gbuffer G-Buffer object to be drawn.
         */
        void RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const;

        void SetViewport();

        void Exterminate() override;

    private:
        GBufferLayer m_gbufferLayer;
        GLProgramHandle* m_geometryHandle = nullptr;

        unsigned int* m_width = nullptr;
        unsigned int* m_height = nullptr;

        LightMgr* m_lightMgr = nullptr;
    };
}